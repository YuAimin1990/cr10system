
import socket
import os
import sys
import struct
import dsptask_pb2
import time

def FileLoad(filepath):
    if os.path.isfile(filepath):
        fp = open(filepath, 'rb')
        data = fp.read()
        if not data:
            print ('{0} no data...'.format(os.path.basename(filepath)))
        fp.close()
    return data

def FileStore(filepath,data):
    if 1:
        fp = open(filepath, 'xb')
        if not data:
            print ('{0} no data...'.format(os.path.basename(filepath)))
            return 
        fp.write(data)
        fp.close()
    return data
    

def SocketClientProcess():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('127.0.0.1', 6666))
    except socket.error as msg:
        print (msg)
        sys.exit(1)
    file_cnt =0   
    while 1:       
        task_data = dsptask_pb2.CallbackData()
        task_data.cmdType = 2
        task_data.opParseParam.netID = 0x32
        cbjsdata=FileLoad('..\\inout\\Callback.json')  
        task_data.opParseParam.cbJsonStr=cbjsdata 
        
        usrjsdata=FileLoad('..\\inout\\CustomParam.json')
        task_data.opParseParam.usrJsonStr=usrjsdata  
        
        task_data.opFInferParam.networkID=0x32
        task_data.opFInferParam.callbackID=0
        intensordata=FileLoad('..\\inout\\Input.bin')
        task_data.opFInferParam.tensorIn=intensordata  
                
        serializeToString = task_data.SerializeToString()
        byte_data = serializeToString
        byte_head = (len(byte_data)).to_bytes(4, byteorder='big')
        print("client->server: protbuf_len:{}".format(len(byte_data)))  
        s.send(byte_head)
        s.send(byte_data)
        
        print("Send over, Waitting Result...");
        t2 = time.time()      
        data_byte  = bytes()  
        while 1:
            head_data = s.recv(4)  
            data_len = int.from_bytes(head_data, byteorder='big')
            if data_len > 0 :
                t3 = time.time()
                data_recv_len = 0
                data_unit_len = 4096
                while data_recv_len < (data_len- data_unit_len):
                    data_unit = s.recv(data_unit_len,socket.MSG_WAITALL) 
                    data_byte = data_byte + data_unit
                    data_recv_len = data_recv_len + data_unit_len
                                             
                data_rem_len = data_len - data_recv_len
                data_rem = s.recv(data_rem_len,socket.MSG_WAITALL) 
                data_byte = data_byte + data_rem   
                t4 = time.time() 
                print ("recv time during") 
                print(t4-t3)
                print ("total time during") 
                print(t4-t2)                
                break
           
        print("server->client: protbuf_len:{}".format(len(data_byte))) 
        task_data = dsptask_pb2.CallbackData()
        task_data.ParseFromString(data_byte) 
        result = task_data.opFInferParam.tensorOut
        print("server->client: cmdType:{}".format(task_data.cmdType))
        print("server->client: status:{}".format(task_data.status))
        print("server->client: result_len:{}".format(len(result)))        
        filename = '..\\inout\\Output'
        filenamepost = '.bin'
        filename = filename + str(file_cnt) +filenamepost
        FileStore(filename,result)       
        file_cnt=file_cnt+1
    s.close()
    
if __name__ == '__main__':
    SocketClientProcess()

