$ARCH = [System.Environment]::GetEnvironmentVariable("PROCESSOR_ARCHITECTURE")

echo $ARCH

if ($ARCH -eq "AMD64" -or $ARCH -eq "x86") 
{
	& {
		Write-Output "x86 detected" -Wait
		Start-Process "bdep" -ArgumentList "init -C @x64-win32-msvc-debug cc config.config.load=build-configs/windows-msvc-debug.config" -Wait
		Start-Process "bdep" -ArgumentList "init -C @x64-win32-llvm-debug cc config.config.load=build-configs/windows-llvm-debug.config" -Wait
		Start-Process "bdep" -ArgumentList "init -C @x64-win32-llvm-release cc config.config.load=build-configs/windows-llvm-release.config" -Wait
	}
}
elseif ($ARCH -eq "ARM64" -or $ARCH -eq "ARM") 
{
	& {
		Write-Output "ARM detected" -Wait
		Start-Process "bdep" -ArgumentList "init -C @arm64-win32-msvc-debug cc config.config.load=build-configs/windows-msvc-debug.config" -Wait
		Start-Process "bdep" -ArgumentList "init -C @arm64-win32-llvm-debug cc config.config.load=build-configs/windows-llvm-debug.config" -Wait
		Start-Process "bdep" -ArgumentList "init -C @arm64-win32-llvm-release cc config.config.load=build-configs/windows-llvm-release.config" -Wait
	}
}
else
{
	Write-Output "Unsupported ARCH detected: $ARCH" 
}