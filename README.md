# OBS Plugin to record CS:GO with Trusted Mode

Since Valve introduced [Trusted Mode](https://blog.counter-strike.net/index.php/2020/07/30991/) you can no longer record Counter-Strike: Global Offensive without using `-allow_third_party_software` in your start parameters.

This is not desireable, as it might have a negative impact on your game experience (Trust Factor).

Signed dlls from `C:\Windows\` are whitelisted, so this plugin will create a folder `obs-studio-hook` and copy the following two signed dlls from OBS there.

```
graphics-hook32.dll
graphics-hook64.dll
```

Further the path will be patched in OBS to load the same dlls but from the new location.

## Installing the Plugin
---
Put `obs-csgo.dll` into the respective folder:
* OBS - `\obs-studio\obs-plugins\64bit`
* Streamlabs - `\Streamlabs OBS\resources\app.asar.unpacked\node_modules\obs-studio-node\obs-plugins\64bit`
