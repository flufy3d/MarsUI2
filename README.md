
![MarsUI-logo](https://flufy3d.github.io/MarsUI/main/marsui.png)

## HTML powered UI and HUD for Unreal Engine 4
(Hit up the wiki for a quick start guide!)

Do you use MarsUI in your project? I'd really love it if you credit me and it! You can even use the [logo right here!](https://res.cloudinary.com/aaronshea/image/upload/v1423576170/MarsUI-Transparent_eu582n.png)

License: MIT

What is it?
---------------------------------------
MarsUI is an Unreal Engine 4 plugin that allows easy interaction with the Chromium Embedded Framework. It provides a simple Material Instance and input functions to help streamline the rendering of rich HTML interfaces.

MarsUI tries to stay out of the way as much as possible. All rendering of the DOM and processing of the JavaScript happens in a separate process, just like Chromium. MarsUI only updates the texture inside the material instance when Chromium requests a redraw, not every tick, saving a bit more processing along the way.

Features
---------------------------------------
+ Chromium Powered (same thing that powers Google Chrome!)
+ Fully compatible with every web technology that Chrome/Chromium works with. (HTML5, WebAudio, WebSockets etc.)
+ No specific ties to ***any*** in game class, simple use Blueprints (or C++) to create a new "MarsUIInst" object and grab its material instance, then you can paint it on anything!
+ Execute JavaScript in the "browser" from your game to pass data to the web page
+ Using `marsui_event` JS native function you can pass data from the page's JavaScript back into UE4!
+ C++ or Blueprints, works with both!

Precautions
---------------------------------------
+ currently only support windows
+ cef version cef_binary_3.2785.1480.g162e9a9_windows64
+ use cmake generate solution file .don't use sandbox
+ copy h5ui_host under the cef_binary_3.2785.1480.g162e9a9_windows64 path
+ add add_subdirectory(h5ui_host) in CMakeLists.txt
+ libcef_dll_wrapper change the build option  /MT for h5ui_host; /MD for UE4
+ when complie in Unreal Project some .h `OVERRIDE` need change to `override`,cef_base.h line 7 11 19; cef_thread_collision_warner.h line 7
+ --allow-file-access-from-files


Setting up the editor and project
---------------------------------------
Then copy the `MarsUI` folder into the "Plugins" folder within your **project** directory, and enable the plugin.

Re-generate your project's Visual Studio file and load up the editor. Then check the plugin list to ensure it has been loaded!

Updating the CEF event loop
---------------------------------------
call a blueprint node to Tick the CEF loop every tick of the level blueprint.


Loading Local Files
---------------------------------------
Set your default URL or use the "Load URL" node/method to load a URL that starts with `marsui://` this will point to the directory root of the project or the game (if packaged). So if you wanted to load an HTML file from `YourProject/UI/file.html`, set the URL to `marsui://UI/file.html`


HUD Example Blueprint
---------------------------------------
Within the release, you'll find an ExampleHUD blueprint file, place this into your project's blueprints directory to try it out! (It's a simple UMG widget pre-configures to accept keyboard and mouse input, with a MarsUI instance hooked up to a canvas)


Shipping Your Game (Linux)
---------------------------------------
Copy all contents of the Linux shipping files into your packaged game's `GameName/Binaries/{Linux}`, these are the required files for the Chromium process.
