# OBS Plugin to record CS:GO with Trusted Mode

Since Valve introduced [Trusted Mode](https://blog.counter-strike.net/index.php/2020/07/30991/) you can no longer record Counter-Strike: Global Offensive without using `-allow_third_party_software` in your start parameters.

[@OBSProject](https://twitter.com/OBSProject/status/1281016442733854725): As a result of today's CS:GO update it is no longer possible to use Game Capture while in trusted mode.

This is not desireable, as it might have a negative impact on your game experience (Trust Factor).

Signed dlls from `C:\Windows\` are whitelisted, so this plugin will create a folder `obs-studio-hook` and copy the following two signed dlls from OBS there.

```
graphics-hook32.dll
graphics-hook64.dll
```

Further the path will be patched in OBS to load the same dlls but from the new location.

## Installing the Plugin

Put `obs-csgo.dll` into the respective folder:
* OBS - `\obs-studio\obs-plugins\64bit`
* Streamlabs - `\Streamlabs OBS\resources\app.asar.unpacked\node_modules\obs-studio-node\obs-plugins\64bit`

## Important Information

* Copying anything to `C:\Windows\` requires admin rights, so for the first time and when OBS updates the dlls, the plugin will ask you to start OBS/Streamlabs with admin rights.
* This will have no effect on VAC/FaceIt since the original signed dlls from OBS are untouched and only copied to a different location.
