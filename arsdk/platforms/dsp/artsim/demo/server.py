import socket
import threading
import time
import sys
import os
import struct
import dsptask_pb2
import google.protobuf

from threading import Thread
from queue import Queue


def CreateSocket():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(('127.0.0.1', 6666))
    except socket.error as msg:
        print (msg)
        sys.exit(1)
    print ("Socket Created Succeed...")
    return s

def BuildConnect(TaskSocket):    
    try:
        TaskSocket.listen(10)
    except socket.error as msg:
        print (msg)
        sys.exit(1)
    print ("Waiting connection...")

    conn, addr = TaskSocket.accept()     
    print ("Accept new connection from ",{addr})
    return conn
  
def WriteMemLendian(mem_addr, value,target):
    value_mod = value % 256  
    try :
        target.write_mem(1, mem_addr, value_mod, 1)         
    except Exception as e :
        print(str(e))  
    value_loop = 0
    while 1:  
        value_loop = value_loop +1    
        value = value // 256       
        if value > 0 :
            value_mod = value % 256
            try :
                target.write_mem(1, mem_addr + value_loop, value_mod, 1)         
            except Exception as e :
                print(str(e)) 
        else:
            break 
def ReadMemLendian(mem_addr,target): 
    value =0   
    byte_num = 4
    while 1:     
        try :
            addr =mem_addr + byte_num - 1
            val_str=target.read_mem(1, addr, 1) 
            val=int(val_str,16) 
            value = value * 256
            value = value + val             
        except Exception as e :
            print(str(e)) 
        byte_num = byte_num -1      
        if byte_num ==0:
            break
    return value
           
  
def TaskRecvFromClient(TaskQueue,TaskConn):
    print("TaskRecvFromClient Start")
    data_byte  = bytes()  
    while 1:
        head_data = TaskConn.recv(4)  
        data_len = int.from_bytes(head_data, byteorder='big')
        if data_len > 0 :
            data_recv_len = 0
            data_unit_len = 4096
            while data_recv_len < (data_len- data_unit_len):
                data_unit = TaskConn.recv(data_unit_len,socket.MSG_WAITALL) 
                data_byte = data_byte + data_unit
                data_recv_len = data_recv_len + data_unit_len
            
            data_rem_len = data_len - data_recv_len
            data_rem = TaskConn.recv(data_rem_len,socket.MSG_WAITALL) 
            data_byte = data_byte + data_rem
          
            task_data = dsptask_pb2.CallbackData() 
            try: 
                task_data.ParseFromString(data_byte)
            except Exception as e :
                print(str(e))
                exit()
                
            TaskQueue.put(task_data);
            break
    print("client->server: protbuf_len:{}".format(len(data_byte)))
    print("TaskRecvFromClient done") 
    return data_len
           
def TaskProcessWithTarget(TaskQueue,data_len,target):
    print("TaskProcessWithTarget Start")  
    WriteMemLendian(0x33801000,data_len,target)
   
    task=TaskQueue.get();
    cmd_id =task.cmdType
    print("CmdID:{}".format(cmd_id))
    WriteMemLendian(0x33801004,cmd_id,target)
  
    while 1:
        if cmd_id == 2: 
            net_id =task.opParseParam.netID
            print("NetID:{}".format(net_id))
            WriteMemLendian(0x33801008,net_id,target)
            
            cbjsonb = bytes()    
            cbjson = task.opParseParam.cbJsonStr 
            cbjsonb = cbjson.encode(encoding='utf-8')    
            print("Len(cbJson):{}".format(len(cbjsonb)))
            WriteMemLendian(0x3380100C,len(cbjsonb),target)
            target.write_mem_new(1, 0x33801010, cbjsonb, len(cbjsonb))
            
            usrjsonb = bytes()    
            surjson = task.opParseParam.usrJsonStr 
            usrjsonb = surjson.encode(encoding='utf-8')  
            print("Len(usrJson):{}".format(len(usrjsonb)))        
            WriteMemLendian(0x33801010+len(cbjsonb),len(usrjsonb),target)   
            target.write_mem_new(1, 0x33801014+len(cbjsonb), usrjsonb, len(usrjsonb))  

            networkID =task.opFInferParam.networkID
            callbackID = task.opFInferParam.callbackID  
            print("NetworkID:{}".format(networkID))  
            print("CallbackID:{}".format(callbackID))     
            WriteMemLendian(0x33801014+len(cbjsonb)+len(usrjsonb),networkID,target)
            WriteMemLendian(0x33801018+len(cbjsonb)+len(usrjsonb),callbackID,target)
            
            addr = 0x3380101C+len(cbjsonb)+len(usrjsonb) 
            for i in range(1):
                intensor = bytes()
                intensor = task.opFInferParam.tensorIn
                print("Len(inTensor):{}".format(len(intensor)))     
                if intensor:
                    addr = addr + i*(4 + len(intensor))
                    WriteMemLendian(addr,len(intensor),target) 
                    addr = addr + 4                
                    target.write_mem_new(1, addr, intensor, len(intensor))                     
                else:
                    break                        
            WriteMemLendian(0x33800000,5,target)     
            break
        else:
            print("Wrong CMDID") 
    print("TaskProcessWithTarget Done") 

def SendResulttoClient(TaskSocket,TaskConn,target):
    print("SendResulttoClient Start")
    t1 = time.time()
    target.resume()
    print("Target Resume Done")      
    ack_cmdid = ReadMemLendian(0x33C02004,target) 
    print("Target->Server:Ack_CmdID:{}".format(ack_cmdid)) 
    ack_status = ReadMemLendian(0x33C02008,target) 
    print("Target->Server:AckStatus:{}".format(ack_status))  
    result_len = ReadMemLendian(0x33C02000,target) 
    print("Target->Server:ResultLen:{}".format(result_len))     
    result =bytes()
    result=target.read_mem_new(1, 0x33C0200C, result_len)
    t2 = time.time()
    print ("SendResulttoClient time during1") 
    print(t2-t1)  
    target.reset()
    print("Target Reset Done")
 
    ack_data = dsptask_pb2.CallbackData()
    ack_data.cmdType = ack_cmdid
    ack_data.status = ack_status
    ack_data.opFInferParam.tensorOut = result
    ack_serializeToString = ack_data.SerializeToString()
    ack_byte_head = (len(ack_serializeToString)).to_bytes(4, byteorder='big')
    TaskConn.send(ack_byte_head)
    TaskConn.send(ack_serializeToString)
    print("SendResulttoClient Done")
    
def DataProcessThread(TaskSocket,target):
    TaskQueue=Queue()
    TaskConn=BuildConnect(TaskSocket) 
    while 1:         
        data_len =TaskRecvFromClient(TaskQueue,TaskConn)
        TaskProcessWithTarget(TaskQueue,data_len,target)
        SendResulttoClient(TaskSocket,TaskConn,target)
       
def SocketServerProcess():
    print("CEVA Simultor ToolEnv Start") 
    if len(sys.argv) < 2 :
        print("Please provide tools location as argument")
        exit()
    tools = sys.argv[1]
    print("Tools location: {tools}")
    sys.path.append(tools + '\\cevatools\\bin')
     
    print ("TCSManager Start")
    import ceva_budi 
    manager = ceva_budi.TCSManager()
    print("Starting the TCSManager...");
    try :
        manager.start()
    except Exception as e :
        print(str(e))
        exit()
    print("Add config file...");
    try :
        manager.config("tcs_config.tcsmdb")
    except Exception as e :
        print(str(e))
        exit()	
    print("Open debug session...");
    try :
        debug_session = manager.debug_sessions()[0]
        debug_session.open()
    except Exception as e :
        print(str(e))
        exit()
    print("Load ARapp.elf...");
    try :
        debug_session.load_app('..\..\dsp_sdk\ARApp\Debug\ARApp.elf')
    except Exception as e :
        print(str(e))
        exit()	
    print("Get targets...");
    try :
        target = debug_session.targets()[0]
    except Exception as e :
        print(str(e))
        exit()
        
    socket = CreateSocket()
    print ("Thread for Task RecvSend Start")
    t = Thread(target=DataProcessThread,args=(socket,target))
    t.start()
    print ("Thread for Task RecvSend End") 


    time.sleep(2000000) 
    print("close the session...");
    try :
        debug_session.close()
    except Exception as e :
        print(str(e))


    print("stop the manager...");
    try :
        manager.stop()
    except Exception as e :
        print(str(e))
    print("CEVA Simultor Server End") 

if __name__ == '__main__':
    SocketServerProcess()
