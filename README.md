# Quake 3: Arena NT #

This is a version of the Quake 3: Arena source code built for x64 & ARM, Direct3D 11 and, optionally, for Windows 8.

http://twitter.com/PJayB

![Pictured: Windows 8 and Surface RT][1]



## About the Port ##

- Supports VS2012 and VS2013
- x86, x64 and ARM architectures
- Direct3D 11.1&2
- XAudio2
- XInput (gamepad support)
- Windows 8 & 8.1
- Widescreen

The goal of this port was to bring Q3A up to latest technology while being as noninvasive as was reasonable. As such it is not a complete rewrite of the original source code targeted at these technologies. This is a good and bad thing. Those familiar with the Q3A source will not find too many surprises, but the engine could be much more efficient had it been rewritten in this way. Also, while it runs atop Direct3D 11, it does not contain any Direct3D 11 specific functionality that couldn't be replicated elsewhere. 

That's not to say it couldn't one day! One of the bonuses of this project is that I have a large platform in which to experiment with making it better/stronger/faster/shinier.

The port took me approximately three weeks in my own time and is not affiliated with my work at Microsoft and neither with Id Software, both of whom are totally unaware of this project.

## Desktop Edition vs. Windows 8 Edition ##

The Desktop Edition, also known as the Win32 edition, is like regular Q3A in that it runs in a window like of old. The Windows 8 build, however, will generate a WinRT package for deployment in the Windows Store (although you can't do that unless you're Id Software). See below for more information on the respective build types.

Both builds have separate build configurations for vanilla Q3A and Quake 3: Team Arena. You can find these under 'Debug' and 'Debug TA' (or Release respectively).

## Building & Running the Desktop Edition ##

You can find the solution for this in code\win32\Quake3Win32_VS201X.sln. Make sure Quake3Win32_VS201X is the Startup Project. Choose the relevant platform and configuration: I prefer x64 and Debug or Debug TA. 

- Drop your _baseq3_ directory into the &lt;root&gt; directory. Same goes for Team Arena's _missionpack_ directory if applicable.
- If launching from Visual Studio, set the Project Properties > Debugging > Working Directory to `$(SolutionDir)..`.
- On the command line, specify `+set sv_pure 0`. This is because you're using the VM DLLs.

## Building & Running the Windows 8 Edition ##

You can find the solution for this in code\win8\Quake3Win8_VS201X.sln.

- Drop your _baseq3_ directory into the &lt;root&gt; directory. Same goes for Team Arena's _missionpack_ directory if applicable.
- Build & Run.

## Enabling or Disabling Features ##

To toggle between XAudio2 or DirectSound use `set snd_driver xaudio` or `set snd_driver dsound`.

To enable or disable the gamepad, use `set in_gamepad 1/0`.

To toggle between Direct3D 11 or OpenGL you can `set r_driver d3d11` or `set r_driver opengl` respectively.

In the desktop version, you can actually do a side-by-side visual comparison of the two renderers. `set r_driver proxy` will do this. 

![Direct3D 11 and OpenGL running side-by-side.][2]

The Windows 8 build is always Direct3D 11, XAudio2, XInput enabled. Networking runs on WinRT sockets.

## ".plan" ##

- Add more configuration settings to the Direct3D 11 layer. At the moment this will just select the best possible settings based on your video capabilities. I'll hook these up to Cvars eventually.
- Optimize the Direct3D 11 layer much more.
- Implement stencil shadows for Direct3D 11.
- Port the QVM code.
- Texture compression (not that it needs it)
- Offline shader compilation
- Shadow solution in D3D11
- New, widescreen menu
- Better keyboard and gamepad UI navigation
- Auto-detect `$(ProjectDir)..` on startup
- Optimize Win8 sockets ('ere be dragons!)

## Known Issues ##

- Win8: Broadcast sockets are not implemented. You'll need to `connect <ip>` from the console to join a game hosted by a Win8 build.
- `r_smp 1` will cause a deadlock on many systems. This is actually a bug in Q3A but I haven't had time to fix it yet.
- Direct3D currently leaks on exit. Sorry about that.
- Cutscene sound is choppy/poppy.
- Controller input doesn't skip cutscenes.
- Cinematics and in-game HUD are stretched in widescreen.
- Dynamic lights clip weirdly in both OpenGL & Direct3D 11. Not sure if this is my fault or not.

## FAQ ##

**Q** I'm seeing `VM_Create on UI failed`. Help!
**A**: This means the UI DLL didn't copy to the right deploy directory. It should be in the same directory as your exe, or in _baseq3_.

**Q**: How do I run the game in widescreen?
**A**: `r_mode -1; r_customwidth 1920; r_customheight 1080`

**Q**: I'm seeing `Cannot find ui/hud.txt`.
**A**: This means you're running normal Q3A with the Team Arena ui.dll. Swap it out for the proper one.

**Q**: Is this a huge graphical improvement?
**A**: It's still the same art, so no. Theoretically it can support textures up to 16K square now though!


**Q**: Is this a huge speed improvement?
**A**: Not yet. 15 years ago games built their graphics command buffers on the CPU and drip-fed them to the GPU. Nowadays we batch up on the GPU as possible up-front to save bandwidth at run-time. In order to be as nonivasive as possible I have emulated the former approach, but this approach doesn't work as well in this day and age. It's a priority for me to fix that, however.

**Q**: How do I know what code you've changed?
**A**: Apart from the Git diffs, you can look for `@pjb` in the source code.

**Q**: Why?
**A**: Because I love Quake and I love code.



  [1]: http://repo.pjblewis.com/q3d3d11/q3withsurfacert.jpg
  [2]: http://repo.pjblewis.com/q3d3d11/q3a-sbs.png
