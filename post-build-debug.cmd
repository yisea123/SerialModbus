@cd %CD%

@set wincc=D:\Infra_G\

@echo ------- Post build step -------
@copy /B /Y %wincc%\panels\para\address_serialMod.pnl panels\para
@copy /B /Y %wincc%\scripts\userDrivers.ctl scripts
@copy /B /Y %wincc%\scripts\userPara.ctl scripts

@echo - restart PVSS driver
pmoncommand -cmd SINGLE_MGR -scmd STOP -args  8 -timeout 4000
wait 50
@del %wincc%bin\*.* /F /Q
@copy /B /Y Debug\*.* %wincc%bin
pmoncommand -cmd SINGLE_MGR -scmd START -args  8 -timeout 200
@echo - restart PVSS driver

@echo ------- Finnish -------
