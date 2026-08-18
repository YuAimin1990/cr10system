
::set  mypath=%~dp0
::echo %mypath%
::cd ..\
::set prepath=%~dp0
::echo %prepath%
set cevatoolpath="C:\CEVA-ToolBox\V18\XM6"
set demopath=C:\Users\ylu\Desktop\ceva_xm6
set cfgpath="C:\Users\ylu\Desktop\ceva_xm6\artsim\cfg"
@echo off
start cmd /k "cd %cevatoolpath% && call CEVA-XM6_set.bat && call CEVA-XM6_set.bat %demopath% && cd %cfgpath% && python ..\demo\server.py %cevatoolpath%"
ping 127.0.0.1 -n 10 > nul 
start cmd /k "cd %cfgpath% && python ..\demo\client.py"

