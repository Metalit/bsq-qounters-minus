# Builds a .zip file for loading with BMBF
& $PSScriptRoot/build.ps1

if ($?) {
    Compress-Archive -Path "./libs/arm64-v8a/libqounters-minus.so", `
        "./libs/arm64-v8a/libbeatsaber-hook_0_8_4.so", `
        "./libs/arm64-v8a/libcustom-types.so", `
        "./libs/arm64-v8a/libquestui.so", `
        "./bmbfmod.json", `
        "./cover.png" `
        -DestinationPath "./qounters-minus_v0.1.0.zip" -Update
}
