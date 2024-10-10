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
@rem 2. Eliminamos la carpeta de qemu
if not exist .\utilidades\qemu\qemu-system-x86_64w.exe goto :deleted
    echo Eliminando carpeta de qemu...
    rmdir /q /s utilidades\qemu

:deleted
echo (1/5) Restaurando archivos comprimidos...
git restore utilidades/qemu.7z.001
echo (2/5) Restaurando archivos comprimidos...
git restore utilidades/qemu.7z.002
echo (3/5) Restaurando archivos comprimidos...
git restore utilidades/qemu.7z.003
echo (4/5) Restaurando archivos comprimidos...
git restore utilidades/qemu.7z.004
echo (5/5) Restaurando archivos comprimidos...
git restore utilidades/qemu.7z.005

:step3
@rem 3. Comprimimos la imagen
set IMAGENAME=minix3hd.plot.%ID%
echo 3.1 - Imagen a comprimir: %IMAGENAME%
set IMAGEZIP=%IMAGENAME%.7z
set IMAGEUNZIP=%IMAGENAME%.qcow2

if not exist .\imagen\%IMAGEUNZIP% goto step3_imagenNoExiste
    echo 3.2 - Imagen unzippeada, zippeando...
    .\utilidades\7za.exe a .\imagen\%IMAGEZIP% .\imagen\%IMAGEUNZIP%
    echo 3.3 - Borrando archivos innecesarios...
    del .\imagen\%IMAGEUNZIP%
    goto step3_Zippeada

:step3_imagenNoExiste
    echo 3.2 - Imagen no existe
    set /p ANS="Was this expected? (y/n): "
    if %ANS%==y goto step4
    goto err_imagenNoExiste
:step3_Zippeada
    echo 3.4 - Imagen zippeada con éxito
    goto step4

:step4
@rem 4. Commiteamos los cambios
git status
goto end

:err_imagenNoExiste
    echo ERROR: Imagen no existe
    goto end

:end
echo All set!