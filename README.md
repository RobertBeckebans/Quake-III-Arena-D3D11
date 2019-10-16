# Quake 3: Arena NT #

This is a version of the Quake 3: Arena source code built for x64 & ARM, Windows 8 and Windows Desktop, Direct3D 11, XAudio2 and XInput.

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

- Drop your _baseq3_ directory into the &lt;root&gt; directory. Team Arena's _missionpack_ is not yet supported.
- Build & Run.

## Enabling or Disabling Features ##

To toggle between XAudio2 or DirectSound use `set snd_driver xaudio` or `set snd_driver dsound`.

To enable or disable the gamepad, use `set in_gamepad 1/0`.

To toggle between Direct3D 11 or OpenGL you can `set r_driver d3d11` or `set r_driver opengl` respectively.

In the desktop version, you can actually do a side-by-side visual comparison of the two renderers. `set r_driver proxy` will do this. 

![Direct3D 11 and OpenGL running side-by-side.][2]

The Windows 8 build is always Direct3D 11, XAudio2, XInput enabled. Networking runs on WinRT sockets.

## ".plan" ##

- Port the virtual machine to ARM and x64 so the game can run with `sv_pure 1` again.
- New, widescreen menu.
- Better keyboard and gamepad UI navigation.
- Auto-detect `$(SolutionDir)..` on startup.
- Optimize Win8 sockets. ('ere be dragons!)
- Texture compression (not that it needs it).
- Implement a shadowing solution in D3D11 to replace the missing one. (`r_shadows 2` will do nothing right now.)
- Shift work off the CPU onto the GPU. (Search for `tess.` in the code for sad times.)
- Fun D3D11 effects :)

## Known Issues ##

- Menu navigation using the gamepad is almost impossible. I'm working on custom code to handle this.
- Win8: Broadcast sockets are not implemented. You'll need to `connect <ip>` from the console to join a game hosted by a Win8 build.
- `r_smp 1` will cause a deadlock on many systems. This is actually a bug in Q3A but I haven't had time to fix it yet.
- Cutscene sound can be choppy/poppy.
- Controller input doesn't skip cutscenes.
- Cinematics and in-game HUD are stretched in widescreen.
- Dynamic lights clip weirdly in both OpenGL & Direct3D 11. Not sure if this is my fault or not.
- HUD models can be stretched/distorted/off center at various aspect ratios.

## FAQ ##

**Q** I'm seeing `VM_Create on UI failed`. Help!
**A**: This means the UI DLL didn't copy to the right deploy directory. It should be in the same directory as your exe, or in _baseq3_.

**Q**: How do I run the game in widescreen?
**A**: `r_mode -1; r_customwidth 1920; r_customheight 1080`

**Q**: I'm seeing `Cannot find ui/hud.txt`.
**A**: This means you're running normal Q3A with the Team Arena ui.dll. Swap it out for the proper one.

**Q**: Is this a huge graphical improvement? 
**A**: It's still the same art, so no. Theoretically it can support textures up to 16K square now though!

**Q**: Have you added loads of cool D3D11 specific shinies?
**A**: Not yet. I wanted to be as non-invasive as possible, at least to start with. Adding funky new things would require many more alterations to the underlying engine first, so it's not a trivial job. See below.

**Q**: Is D3D11 a huge speed improvement over OpenGL?
**A**: I'm using D3D11 to emulate the way things were done in 1999, so the performance is equivalent to OpenGL. Dramatic wins could be achieved by storing geometry on the GPU instead of clipping and transforming it on the CPU, but Quake 3 takes extensive advantage of OpenGL 1's immediate nature. Changing that would require substantial rewriting of the rendering code. I have considered starting from the ground up on a daily basis since starting this project, but I've yet to cave in to this temptation.

**Q**: What's the performance like on the Surface RT?
**A**: 30+ fps, averaging 42. It copes less well with mirrors and portals, dropping to 15.

**Q**: How do I know what code you've changed?
**A**: Apart from the Git diffs, you can look for `@pjb` in the source code.


  [1]: http://repo.pjblewis.com/q3d3d11/q3surface.jpg
  [2]: http://repo.pjblewis.com/q3d3d11/q3a-sbs.png
