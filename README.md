# OBS Plugin to capture CS:GO with Trusted Mode enabled

Ever since Valve introduced [Trusted Mode](https://blog.counter-strike.net/index.php/2020/07/30991/) you could no longer record Counter-Strike: Global Offensive without using `-allow_third_party_software` in your start parameters.

[![@OBSProject](https://user-images.githubusercontent.com/173158/115598587-dd0b2f00-a2da-11eb-849d-6c68c6675962.png)](https://twitter.com/OBSProject/status/1281016442733854725)


This is not desireable, as it might have a negative impact on your game experience (Trust Factor).

## How it works

Signed dlls from `C:\Windows\` are whitelisted, so this plugin will create a folder `obs-studio-hook` and copy the following two signed dlls from OBS there.

```
graphics-hook32.dll
graphics-hook64.dll
```

Further the library path in OBS will be patched to load the same dlls but from the new location.

## Installing the Plugin

Put `obs-csgo.dll` into the respective folder:
* OBS - `\obs-studio\obs-plugins\64bit`
* Streamlabs - `\Streamlabs OBS\resources\app.asar.unpacked\node_modules\obs-studio-node\obs-plugins\64bit`

## Important Information

* Copying anything to `C:\Windows\` requires admin rights, so for the first time and when OBS updates the dlls, the plugin will ask you to start OBS/Streamlabs with admin rights.
* This should have no effect on VAC / FaceIt / etc. since the original signed dlls from OBS are untouched and only copied to a different location.
