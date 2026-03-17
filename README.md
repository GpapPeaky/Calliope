# CoBaLT (v2.0.0)

**The Console-Oriented Basic Line Transformer** (formerly known as Muse) is a lightweight console-based editor that allows users to manage files, directories, and editor configurations directly from a command-line interface. The editor supports switching between console mode and insert mode seamlessly, along with a wide range of file, directory, and editor management directives. The basic idea behind it is that today's editor are simply too bloated with unwanted features, at the other hand switching to something like vim takes years of experience to fully master and to work productively. So with that in mind, I set out to capture the best of both worlds, notably the **lightweightness, customization, handilng** and **visual simplicity** of vim, vi like editors, as well as the **flexibility** of modern editors like VS Code.

<p align="center">
  <img src="assets/icon/mdmuse.png"/>
</p>

<p align="center">
  <img src="assets/md/md.png"/>
</p>

---

## 1. Features

### 1.1 Console Mode

  The console's primary use is for executing commands, these commands are what I decided to call *directives*.

  Directives can be distinguished into **two notable types** depending on the context:
      
  #### 1.1.1 Switch-To-File/NQ directives
  
  This sort of directives are invoked whenever we want to switch/enqueue a new file in the editor's virtual filesystem queue, simply by **typing the name of the file in the console**

  #### 1.1.2 Command directives

  These directives are the main culprit behind more complex **file/directory handling and management, info, guides and auxillary functions**. Command directives will elevate your typing experience as they are the simplest and most common way to **capture the complexity of the aforementioned behaviours**. The most common ones, are also tied to shortcuts, so we can improve typing speeds (*as vim-like as possible*). I recommend visiting the relative section in the README for more info on the different directives

  ##### 1.1.2.1 Infile Directives

  There are **some command directives** that can be sub-categorised as *infile directives*, which are mostly manipulating an infile attribute, such as
  ```c
  :g L // Go to line L
  ```
  ```c
  :ge // Go to the end of the file
  ```
  ```c
  :gs // Go to the start fo the file
  ```

  ##### 1.1.2.2 Filesystem Directives

  These directives are tasked with **interacting with the virtual file system**, writting, removing files and directories, navigation etc. Some useful ones include
  ```c
  :w // Write current open file (more on the file system later)
  ```
  ```c
  :c s // Create a new file with name s
  ```
  ```c
  :cd // Change directory, allows you to traverse freely your local folders
  ```

  ##### 1.1.2.3 System Directives

  System directives are designed to be cross-platform directives so they can execute some very resrtricted system routines. A very notable example is the **native explorer**, which needs different specifications based on the OS. Can be invoked as
  ```c
  :o // Open the native folder explorer
  ``` 
  ##### 1.1.2.4 Virtual Filesystem/FileQueue Directives

  These directives enable a faster interactive option for the many open files you may have. These mostly work with the File Queue which we will later analyze in the *Filesystem* section
  ```c
  :qa // Dequeue all files in the FileQueue
  ```

### 1.2 Insert Mode

  This mode is the main way of *infile* interactions ranging from basic typing, to deleting lines instantly and handling multiple cursors
  
  #### 1.2.1 Multi-cursor Support

  Although still experimental we can generate trailling and leading cursors the same way as vs code and interact with them. Note to remove all cursors **(Secondaries, with cursor_id != 0)** we need to press **LCtrl + P** so we return to a safe *cursor-space*.

  #### 1.2.2 Indentation
  A must for typing speeds, a very simple indentation and opener/closer autocompleting algorithm. 

  #### 1.2.3 Cursor Fragment
  The word the cursor is always on top of is displayed at the top bar of the UI

  #### 1.2.4 Infile Autocomplete
  The editor checks and shows suggestions to autocomplete to, based on the current cursor fragment. It checks and updates a suggestions vector, while also it retokenizes when a change is made. It works parallel to the token highlighting cache, which
  is spilt to tokens per lines, the only difference being that the autocomplete uses **only identifiers and language keywords, loaded as unique elements**. The retokenizing basically *hashes* inside the dirty line, and updates the tokens for both the highlighting and the autocomplete suggestions. See **1.3.2.2** for a better explaination regarding tokens.

### 1.3 File Abstractions
  This is **the very elemental type of object** that the editor interacts each second, even the built-in console is seen as 1 line file which we can write to and execute. There exists a **special kind of file that exists ONLY in memory (NAF, No Active File)** which is used as a fall back when the FileQueue is empty, but more on that later.

  #### 1.3.1 File Specifications
  The file object is designed to be as simple as possible, it includes as little info as possible apart from its lines (file's content). we keep a *path*, a *name*, a *dirty* indicator (saved/unsaved) as well as some other data structures that are a must for highlighting.

  #### 1.3.2 Highlighting And Language Support
  I wanted the **highlighting** to be as **uniform and simple** as possible in all **61 languages** currently supported, this of course comes with some extra complexity regarding **tokenization, retokenization, rendering and performance**

  ##### 1.3.2.1 Language Support
  Each time a new file is loaded into the *FileQueue*, the editor assigns it a file extension enumerator, which is later used in ordered to acquire the keyword set, which is checked for lanauge keyword highlighting, very **simple, flexible and expandable** for a more complex highlighting algorithm. The file extesion also **defaults to .txt** if it is not recognized 

  ##### 1.3.2.2 Tokenization
  After loading the language's keyword set, based on the file extesion, it scans the file
  and keeps a **2D vector of tokens** which are basically a **pair of a string and colour**. This is a **tokens per line system**, which accelerates performance whenever the file needs to be retokenized, in coordination of course with an unordered set of recently affected lines

  #### 1.3.2.3 Retokenizaton And Dirty Line Caching
  As previously stated, since the tokens are organized *per line* and not *per file*, we accelerate retokenization whenever a line becomes dirty. we hash into token vector, and **simply retokenize the new line's contents!** Dirty lines are saved in an unordered set field inside the file object

### 1.4 File Queue And Virtual Filesystem
  The FileQueue is a very **fundamental** part o the editor since all open files are located there, allowing easier and faster access for the user. Traversing it is trivial, using the **LCtrl + > or LCtrl + <** shortcut. Files can be loaded by a **Switch To File/NQ Directive** through the console. It is important to note that files enqueued, exist as copies of the actual real ones, so in a more practical sense the FileQueue also works as a virtual filesystem. For instance deleting an enqueued file (Deleting from a directory) and then saving it, it creates in again! Created files are also instantly loaded into the FileQueue. Enqueued files can be seen at the bottom of the editor window!

  #### 1.4.1 FileQueue Handling
  The FileQueue can be handled very easily, through shortcuts and directives such as the **LCtrl + q** shortcut and the
  ```c
  :q    // Close, Dequeue current viewing file 
  :wq   // Write and dequeue
  :qa   // Dequeue the entire loaded file queue
  :qas  // Dequeue only the clean (saved) loaded files (A bit safer) 
  :wqa  // Write and dequeue all loaded files
  ```

  #### 1.4.2 NAF 
  A simple global file object, used as a **fallback** for functions whenever the **FileQueue size is 0**

### 1.5 Shortcuts Guide
  For simplifying the editor's use, This list will include all possible shortcuts and their invoked effect


### 1.6 Directives Guide
  Here we will anylize all directives and their invoked effect

---

## Building and Installation

You can either download the compiled build in the **releases** section, or compile it manually. Note that it requires compiling both raylib and source with a **64-bit compiler**.

## License

```
Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License

Copyright (c) 2025 CoBaLT Project Contributors

=======================================================================================================================================

This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.

You are free to:

  * Share — copy and redistribute the material in any medium or format
  * Adapt — remix, transform, and build upon the material

Under the following terms:

  * Attribution — You must give appropriate credit, provide a link to the 
    license, and indicate if changes were made. You may do so in any 
    reasonable manner, but not in any way that suggests the licensor 
    endorses you or your use.

  * NonCommercial — You may NOT use the material for commercial purposes.

  * ShareAlike — If you remix, transform, or build upon the material, you 
    must distribute your contributions under the same license as the original.

  * No additional restrictions — You may not apply legal terms or 
    technological measures that legally restrict others from doing anything 
    the license permits.

Notices:

You do not have to comply with the license for elements of the material in 
the public domain or where your use is permitted by an applicable exception 
or limitation.

No warranties are given. The license may not give you all of the permissions 
necessary for your intended use. For example, other rights such as publicity, 
privacy, or moral rights may limit how you use the material.

================================================================================

For commercial licensing inquiries, please contact: [your-email@example.com]

Full license text available at:
https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode

```

---

## Credits

**Developed by GpapPeaky | Giorgos Papamatthaiakis**