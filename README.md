# OBS Plugin to capture CS:GO with Trusted Mode enabled

[![@OBSProject](https://user-images.githubusercontent.com/173158/115598587-dd0b2f00-a2da-11eb-849d-6c68c6675962.png)](https://twitter.com/OBSProject/status/1281016442733854725)

Ever since Valve introduced [Trusted Mode](https://blog.counter-strike.net/index.php/2020/07/30991/) you could no longer capture Counter-Strike: Global Offensive without using `-allow_third_party_software` in your start parameters.

This is not desireable, as it might have a negative impact on your game experience (Trust Factor).

## 🚨 [A notice from Valve / McJohn](https://www.reddit.com/r/GlobalOffensive/comments/mvldce/i_created_an_obs_plugin_to_capture_csgo_with/gveoqgc?&context=3) 🚨

> We will close down this particular loophole because (per the FAQ) one of the goals of Trusted mode is to make it trivial to VAC ban someone who has software inside the process when they have claimed to be running without having any third party software injected into the CS:GO game process. Trusted Mode has been extremely successful in this regard.
>
> I highly recommend AGAINST using this, or any other Trusted Mode injection bypass. Using any method to bypass Trusted Mode protections that result in third party code being injected into CS:GO can and will catch you a VAC ban, which absolutely will not be rescinded.
>
> If you want to stream while playing CS:GO, simply add the launch option
>
> -allow_third_party_software
>
> OBS will happily inject in the normal way once you've done that.

## How it works

Signed dlls from `C:\Windows\` are whitelisted, so this plugin will create a folder `obs-studio-hook` and copy the following two signed dlls from OBS there.

```
graphics-hook32.dll
graphics-hook64.dll
```

Further the library path in OBS will be patched to load the same dlls but from the new location.

## Download

[Go to the releases page.](https://github.com/luastoned/obs-csgo/releases)

## Installing the Plugin

Put `obs-csgo.dll` into the respective folder:
* OBS - `\obs-studio\obs-plugins\64bit`
* Streamlabs - `\Streamlabs OBS\resources\app.asar.unpacked\node_modules\obs-studio-node\obs-plugins\64bit`

## FAQ / Important Information

* Why does the popup require admin privileges?
  * Copying anything to `C:\Windows\` requires admin privileges, so for the first time and when OBS updates the dlls, the plugin will ask you to start OBS/Streamlabs with admin privileges.
* Will VAC / FaceItAC / etc. ban me?
  * This should have no effect on VAC / FaceIt / etc. since the original signed dlls from OBS are untouched and only copied to a different location.
* CS:GO crashes when the plugin is installed
  * It looks like this is a bug with Nvidia Ansel since one of their dlls gets blocked by CS:GO.
You will need to disable Ansel to stop CS:GO from crashing.
In your `C:\Program Files\NVIDIA Corporation\Ansel[\Tools]` you can find `NvCameraConfiguration.exe` application (needs admin privileges to run). After running it, a dialog window is displayed and in the bottom part you can find Ansel status frame, check disabled there, then Save.
![grafik](https://user-images.githubusercontent.com/173158/115627663-af84ac80-a2ff-11eb-8b3e-dffad9fb11af.png)


## Donations

[![buymeacoffee](https://www.buymeacoffee.com/assets/img/guidelines/download-assets-sm-1.svg)](https://buymeacoffee.com/luastoned)

## Screenshots

![OBS](https://user-images.githubusercontent.com/173158/115609778-434a7e80-a2e8-11eb-8a37-4425c8008130.png)
![Streamlabs](https://user-images.githubusercontent.com/173158/115610053-a2a88e80-a2e8-11eb-914d-a90f14727940.png)
