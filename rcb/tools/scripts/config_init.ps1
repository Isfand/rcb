function Find-Root
{
	$dir = $PSScriptRoot
	while ($dir -ne [System.IO.Path]::GetPathRoot($dir))
	{
		if (Test-Path "$dir\packages.manifest")
		{
			return $dir
		}
		$dir = Split-Path -Parent $dir
	}
	Write-Error "ERROR: project root not found"
	return $null
}

$RROOT = Find-Root
if (-not $RROOT)
{
	Write-Output "Aborting"
	exit 1
}

$RCB_OUT_DIR = "$RROOT\artefacts\rcb"
$ARCH = [System.Environment]::GetEnvironmentVariable("PROCESSOR_ARCHITECTURE")

Write-Output $ARCH

if ($ARCH -eq "AMD64" -or $ARCH -eq "x86")
{
	& {
		Write-Output "x86 detected" -Wait
		Start-Process "bdep" -ArgumentList "init -C $RCB_OUT_DIR\x64-win32-msvc-debug @x64-win32-msvc-debug cc config.config.load=$RROOT\rcb\tools\build-configs\windows-msvc-debug.config" -Wait
		Start-Process "bdep" -ArgumentList "init -C $RCB_OUT_DIR\x64-win32-llvm-debug @x64-win32-llvm-debug cc config.config.load=$RROOT\rcb\tools\build-configs\windows-llvm-debug.config" -Wait
		Start-Process "bdep" -ArgumentList "init -C $RCB_OUT_DIR\x64-win32-llvm-release @x64-win32-llvm-release cc config.config.load=$RROOT\rcb\tools\build-configs\windows-llvm-release.config" -Wait
	}
}
elseif ($ARCH -eq "ARM64" -or $ARCH -eq "ARM")
{
	& {
		Write-Output "ARM detected" -Wait
		Start-Process "bdep" -ArgumentList "init -C $RCB_OUT_DIR\arm64-win32-msvc-debug @arm64-win32-msvc-debug cc config.config.load=$RROOT\rcb\tools\build-configs\windows-msvc-debug.config" -Wait
		Start-Process "bdep" -ArgumentList "init -C $RCB_OUT_DIR\arm64-win32-llvm-debug @arm64-win32-llvm-debug cc config.config.load=$RROOT\rcb\tools\build-configs\windows-llvm-debug.config" -Wait
		Start-Process "bdep" -ArgumentList "init -C $RCB_OUT_DIR\arm64-win32-llvm-release @arm64-win32-llvm-release cc config.config.load=$RROOT\rcb\tools\build-configs\windows-llvm-release.config" -Wait
	}
}
else
{
	Write-Output "Unsupported ARCH detected: $ARCH"
}