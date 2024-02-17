

# GlummysPCBS2ModdingToolsuite
A small modding tool suite built for IL2CPP Unity games

* Bare bones mod loader that supports loading certain "**plugins**"

* Can generate basic Visual Studio 2022 projects to hook to specified functions using Minhook

* Can be used to quickly receive data from a dumped script.json file based off of a Game's GameAssembly.dll

* Can be used as a basic ASI loader for any game


# Requirements for running the mod loader and plugins
* .NET 8.0 Runtime

* A Windows version that supports .NET 8.0 

* A game that is using Il2CPP, games that don't use unity or use mono can still run the modloader in the "**ASI loader**" mode

# Requirements for creating plugins and building the tool suite
* Visual Studio 2022 or < 

* .NET 8.0 Runtime/SDK

* VCPKG

* The following VCPKG packages:<br/>
Minhook ```vcpkg install minhook_x64-windows-static```<br/>
Nlohmann-json  ```vcpkg install nlohmann-json_x64-windows-static``` 

# Create your first plugin template
In this section I will go over on how we can create a plugin using the tool suite.<br/>
First we need to make sure that we have all the requirements listed above, then you go ahead and download the latest tool suite [release](https://www.nexusmods.com/pcbuildingsimulator2/mods/55).

After you have downloaded the tool suite, you want to extract it into the game's main directory and simply launch the game. Wait for it to dump the assembly and exit right after.

That's it, you are done with the initial setup!<br/>
Now we get to the fun part...<br/>
Locate the "AddressGetter" folder inside your game directory, launch the "**AddressGetter.exe**" and follow these steps:<br/>
* Launch the game with the mod loader installed, or dump the game assembly on your own

* Find the dumped script.json (you can find it in the "**DumpedIL2CPP**" folder which is located in the game directory as well if you chose to launch the game instead of manually dumping), copy it's path and enter it in the prompt of the AddressGetter
  
* Now you need to know which functions you want to hook/modify, if you don't know what you want to hook to, take a look into the dummy .dll's the dumper has generated for you (they can be found in **DumpedIL2CPP\DummyDll**)

* The next prompt is kind of obsolete and just creates a small dump of the offsets for the specified functions, simply skip if you don't need a quick dump of the offsets

* All found functions will be displayed now, they are displayed and identified by their name, signature and offset/address (note that functions and their corresponding class will be displayed as follows: returnType ClassName_FunctionName(Parameters))

* Now the AddressGetter will ask you if you want to create a template project for the functions, I improved upon this a lot and it's highly recommended to use it, so we enter 'y' for yes

* Now it prompts for a project name, simply enter anything you want (note that special signs cause issues, please limit yourself to: '_', '-' and '.')

* Now you specify the location where the project will be created in, it will create a new folder in that location with your project name and the template inside of that

* Now it asks you if you want to auto-generate hooks, this can cause problems with some functions but also makes it a little faster to write plugins, I always choose 'y' for this, but you can skip it if you wish with 'n' 

* That's it, you now created your first template, but we aren't done yet!

# Use your created template to create your first plugin
Now we will go over the true fun part, open your created template in any IDE you want, I recommend Jetbrains Rider or Visual Studio 2022 for this.<br/>

**This section assumes that you have all requirements satisfied**

Upon opening, you first want to set the build target to Release-X64.<br/>
After you have done that, you want to go into the "**hooks.h**" header file and look at it for a moment.<br/>

You will quickly notice how you got a lot of errors, well they are there because the project doesn't know of the types from the game, in my case I have unknown types such as: "PCSaleInfo_o", "Job_o", "MethodInfo".<br/>
Don't worry though, this is expected and can be easily fixed, for all types that aren't recognized, you simply want them to replace with "**DWORD**", to quickly replace types instead of going 1 by 1, you can press ctrl + H to open the "**Replace**" dialog in your IDE (please note that this hotkey can be different depending on your IDE config)

After you have done all that, you want to add an existing item to the project, what you want to add then is the following: A file called yourProjectName.offsetRequests.json, this file will act as a sort of API request to our mod loader which will populate the json file with data needed to hook the functions, it should already have default values which are from your last game session before creating the template. 

After you added the json file, you want to set it to always copy upon build process, how to do that also depends on your IDE, simply google: "How to copy a file to output directory when building (YOUR IDE)".

Good, very good even. Now we can build (make sure Release-X64 is selected) and copy the output files (yourProjectName.dll, yourProjectName.offsetRequests.json) to the plugins folder of our game, that's it, you have now made a plugin!

Obviously though, we didn't do any modifications to the code, all we do is print out a message when the function/s we wanted to hook are called, here is an example of one of my plugins to illustrate what I mean:
```C++
int32_t(__fastcall* PCSaleInfo_GetCashForOverclock_o)(DWORD*, bool, const DWORD*);

int32_t __stdcall PCSaleInfo_GetCashForOverclock_hook(DWORD* __this, bool Overclocked, const DWORD* method)
{
    int32_t returnResult = PCSaleInfo_GetCashForOverclock_o(__this, Overclocked, method);
    
    // Print called function
    QUICKDEBUG("PCSaleInfo_GetCashForOverclock called with result: " << returnResult);
    
    return returnResult;
}

```

As you an see, we print "PCSaleInfo_GetCashForOverclock called with result: returnResult" every time this gets called, if you want to add custom logic, you simply put the code between the returnResult and the return statement, you can even overwrite the return result to completely alter behavior of the game code, here is the above example but it displays a message box when it gets called, instead of boring text in the console
```C++
int32_t (__fastcall*PCSaleInfo_GetCashForOverclock_o)(DWORD*, bool, const DWORD*);

int32_t __stdcall PCSaleInfo_GetCashForOverclock_hook(DWORD* __this, bool Overclocked, const DWORD* method)
{
    int32_t returnResult = PCSaleInfo_GetCashForOverclock_o(__this, Overclocked, method);
    MessageBox(NULL, "PCSaleInfo_GetCashForOverclock called", "A hook got called!", MB_OK);
    return returnResult;
}

```

And that's it with the short tutorial on how to create a plugin with custom behavior!

# What is an offsetRequests.json file?
An offsetRequests.json file is essentially like an API request you could say.<br/>
Let me explain it a little further with this example (from a plugin I made which is available [here](https://github.com/Glumboi/PCBS2-InfiniteMoney/tree/main):<b/>
```json
{
  "caller": "PCBS2-InfiniteMoney",
  "offsetRequests": [
    {
      "searchName": "CareerStatus_GetCash",
      "value": "0xFFFFFF",
      "signature": "int32_t CareerStatus__GetCash (CareerStatus_o* __this, const MethodInfo* method);"
    }
  ]
}
```

So, as with all json files, we have one root node, under that we have 2 fields, 1 being our "**caller**" and the other being an array of objects which basically represent the targets the plugin wants to hook to. Now let's get into a little more detail on what these fields do:<b/>

* "**caller**" This essentially is your plugin's name and is only relevant for debugging purposes really, make sure that it fits your plugin's name though!

* "**offsetRequests**" As said before, this is a collection of requests we "send" towards the mod loader, the mod loader will start an instance of the AddressGetter in the so called "internal" mode with the json file in it's arguments, the AddressGetter will then fill out each request with the proper data to do a successful hook 

* "**Request field: searchName**" This is essentially the class and name of a function separated by an underscore, this was used to find functions before, now it's primary used as like a "double check" to make sure the AddressGetter returns the exact thing we asked for, make sure that it is exact to whats in the signature but with only a single underscore instead of 2

* "**Request field: value**" This field will hold the address/offset, should be pretty self explanatory

* "**Request field: signature**" This field holds the function's return type, the class + name and arguments, it is used in combination with the "signature" field to get a unique string to search for in the script.json file which holds every dumped function of a game and is used to "double" check the request

# What is and how do I use the AddressGetter?
The AddressGetter is nothing short of just a simple text parser, it reads through a file in the given arguments and returns re formatted data, there 2 primary ways to use this, 1 of which has a couple different outcomes for the output and the other is purely for the mod loader to fulfill requests.<b/>

So how does it work?

### Well, let me explain the "standalone" way first.

Upon launching the AddressGetter.exe (with no arguments) you will see a CLI asking you for a script.json file, as I stated somewhere above already, you put the game's dumped script.json file path from the IL2CPP dumper in there.

Now it asks for functions to look out for, here you enter any function you would like to hook to, if you want to hook to multiple, separate them with a comma (like so: functionA,functionB,functionC), if you want to hook to multiple overloaded functions, only 1 name is needed, the AddressGetter will list all functions it finds with that name.

Now it wants a path from you, you can simply skip this though, this specific thing was meant as a fast debugging function and didn't get removed from it yet

After printing all the functions it found, you will be asked if you want to create a project for the found functions, if you wish to do so, simply enter 'y'. You can also manually create your own project and use a json file you setup yourself using the printed data, for that just save the printed functions somewhere and enter 'n'.

If you chose 'y', then now it's time to choose a name, as stated above already, you can enter anything, as long as it doesn't hold any special characters, except for: '-', '_', '.'.<br/>
After entering the name, it also wants a path from you, this is a path to a directory it will create the project in, projects will be stored in a folder which has the name of your project inside the now specified directory. 

### Now it's time to explain the "internal" way.

This is mostly relevant for the mod loader, but I will go over it regardless.

The AddressGetter can be ran in "internal" mode using the following arguments:

```AddressGetter.exe internal <your offsetRequests.json file> <path to the script.json file> | useDefault <stealthPrint: bool>```

Ok, but what do they mean?

* "**Argument: internal**" The first one is pretty obvious and explains itself, this will tell the AddressGetter to run in the "internal" mode

* "**Argument: requestFile**" The second one consists of the offsetRequests.json file you would like to populate

* "**Argument: scriptJson**" The third contains the path to the dumped script.json file to use for the requests, the part after the ' | ' and the '|' itself should not be included, '|' means "or" which basically means that you can manually specify this path or let the AddressGetter use the default location of the dumped data, the default location for the dumped data is: ```.\DumpedIL2CPP\script.json```

* "**Argument: stealthPrint**" This argument allows for a more minimalistic debugging, "true" is what the mod loader uses


# "Design guidelines" for plugins

* If printing to the console, make sure to use the QUICKDEBUG macro or the functions from the **debugUtils.h** header file, or simply make sure to put your message in this format: [yourProjectName]: YourMessage <br/> 
This is to ensure a cleaner look and an easy to identify debugging

* Remove all debugging code from your plugin once done with working on it, this includes the following code parts:<br/>
```C++
    //Print all offsets for debugging, remove when you are done testing your plugin!
    for (auto& offset : functionOffsets)
    {
        QUICKDEBUG("Offset loaded: " << offset.searchName << " at " << offset.value);
    }

```
This can be left in, but removing it certainly removes some useless clutter in the console

```C++
    //Load addresses from json file
    QUICKDEBUG("Loading offsets from " << JSONFILE);

```

# This is it for now, thank you for reading til here!
