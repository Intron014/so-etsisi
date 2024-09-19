@echo off

set ASSIGNEDLETER=M

:step1
@rem 1. Leemos el número de matrícula
set /p ID=<id.txt
findstr /C:"TODO" id.txt >nul
if %errorlevel%==0 (
    goto the_end_err_no_id
) else (
    set /p ID=<id.txt
)
echo 1.1 - ID: %ID%

@rem Variables necesarias:
set IMAGENAME=minix3hd.plot.%ID%
set IMAGEZIP=%IMAGENAME%.7z
set IMAGEUNZIP=%IMAGENAME%.qcow2

if "%1"=="--minix" (
    goto step4
)

:step2
@rem 2. Extraemos la carpeta de qemu
if exist .\utilidades\qemu goto step2_carpetaExiste
    echo 2.1 - Qemu zippeado, unzippeando...
    .\utilidades\7za.exe -o".\utilidades\" -aoa  x .\utilidades\qemu.7z.001
    echo 2.2 - Borrando archivos comprimidos...
	del utilidades\qemu.7z.*
    goto step2_carpetaUnzippeada

:step2_carpetaExiste
echo 2.1 - Qemu OK
goto step3
:step2_carpetaUnzippeada
echo 2.3 - Qemu unzippeado con éxito
goto step3

:step3
@rem 3. Extraemos la imagen almacenada
echo 3.1 - Imagen a extraer: %IMAGENAME%
@rem .\imagen\minix3hd.xxx.%id%.qcow2 Esta será la imagen de trabajo.
if exist .\imagen\%IMAGEUNZIP% goto step3_imagenExiste
    echo 3.2 - Imagen zippeada, unzippeando...
    .\utilidades\7za.exe -o".\imagen\"  e .\imagen\%IMAGEZIP%
    goto step3_imagenUnzippeada


:step3_imagenExiste
echo 3.2 - Imagen OK
goto step4
:step3_imagenUnzippeada
echo 3.3 - Imagen unzippeada con éxito
goto step4

:step4
@rem 4. Preparamos e iniciamos
tasklist /FI "IMAGENAME eq httptar.exe" 2>NUL | find /I "httptar.exe" >NUL
if %ERRORLEVEL% == 0 goto httptar_ok
    echo ERROR No se encuentra el servidor HTTPServer
    echo Abre otra terminal y open it...
    pause


:httptar_ok
echo Init Temp
set RUTATRABAJO=.\
subst %ASSIGNEDLETER%: %RUTATRABAJO%
echo %ASSIGNEDLETER%: %RUTATRABAJO%
set MINIX_PATH=%ASSIGNEDLETER%:\imagen\%IMAGEUNZIP%
echo Trabajaremos con %MINIX_PATH%

set MINIXOUT=.\out
if exist %MINIXOUT% goto out_dir_created
    mkdir %MINIXOUT%

:out_dir_created

echo All checks OK
echo Booting MINIX

%ASSIGNEDLETER%:utilidades\qemu\qemu-system-x86_64w.exe ^
-cpu "pentium3" ^
-m 512 ^
-name %ID% ^
-rtc base=localtime ^
-hda %MINIX_PATH% ^
-drive file=fat:rw:%MINIXOUT%,format=raw,media=disk,cache=none ^
-netdev user,id=n1,ipv6=off,restrict=off,hostfwd=tcp:127.0.0.1:5522-:22 -device ne2k_pci,netdev=n1,mac=52:54:98:76:54:32 ^
-debugcon file:.\log_e9.bin

echo Has finalizado la ejecución de Minix

:step4_end
subst /d %ASSIGNEDLETER%:
echo %ASSIGNEDLETER% has been released
goto end

:the_end_err_no_id
echo ERROR: No se ha encontrado la matricula en id.txt

:end
echo All set!