@echo off
set current_dir=%cd%

if not exist build md build
cd build
:: rd /s /q %cd%
@echo on
cmake -G "Visual Studio 14 Win64" ../xpy
@echo off

cd %current_dir%
