param (
    [Parameter(Mandatory=$false)]
    [Switch]$useDebug,
    [Parameter(Mandatory=$false)]
    [Switch]$log
)

& ./build.ps1
if($?) {
    if ($useDebug.IsPresent) {
        & adb push build/debug/libsongloader.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/libsongloader.so
    } else {
        & adb push build/libsongloader.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/libsongloader.so
    }
    & adb shell am force-stop com.beatgames.beatsaber
    & adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
    if ($log.IsPresent) {
        & ./log.ps1
    }
}