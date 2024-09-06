msbuild
if ($?) {
    $env:PATH += ";C:\GameDev\OpenGL\dll;C:\GameDev\SDL\dll"
    .\Debug\ComponentFramework.exe
}
