@echo off


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
set IMAGENAME=minix3hd.plot.%ID%
echo 3.1 - Imagen a extraer: %IMAGENAME%
set IMAGEZIP=%IMAGENAME%.7z
set IMAGEUNZIP=%IMAGENAME%.qcow2
@rem .\imagen\minix3hd.xxx.%id%.qcow2 Esta será la imagen de trabajo.
if exist .\imagen\%IMAGEUNZIP% goto step3_imagenExiste
    echo 3.2 - Imagen zippeada, unzippeando...
    .\utilidades\7za.exe -o".\imagen\"  e .\imagen\%IMAGEZIP%
    goto step3_imagenUnzippeada


:step3_imagenExiste
echo 3.2 - Imagen OK
goto end
:step3_imagenUnzippeada
echo 3.3 - Imagen unzippeada con éxito
goto end

:the_end_err_no_id
echo ERROR: No se ha encontrado la matricula en id.txt

:end
echo All set!