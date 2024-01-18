System Information Fetching Kernel Module
===

NYCU Operation System aut 2023 Homework3

* [Assignment 3: System Information Fetching Kernel Module](https://hackmd.io/@a3020008/r1Txj5ES6)

### How I implemented the program 
### Open and Release Operations
* Open Operation (device_open):

This function should check if the device is already open (to prevent multiple accesses) and set any initial state or protections needed.
Example: Use a flag like Device_Open to track if the device is already in use. If it is, return an error; otherwise, set the flag and initialize as needed.

* Release Operation (device_release):

This function is called when the device is closed. It should reset any state and remove protections set up in the open operation.
Example: Reset the Device_Open flag and perform any necessary cleanup.
### Write Operation

* Write Operation (device_write):
This operation should accept a mask from the user, which determines the data returned by the read operation.
Example: Define a global mask variable. In device_write, update this mask based on the user input.

### Read Operation

* Read Operation (device_read):

This function should return data based on the current mask setting. The data includes a logo and various system information.
The first line should always be the machine hostname.
The second line is a separator, the length of which equals the hostname length.
Subsequent lines depend on the mask, showing different system information like CPU model, memory, etc.
Include functions to fetch each piece of information based on the mask.
Handling the Logo:
```
        .-.        
       (.. |       
       <>  |       
      / --- \      
     ( |   | |     
   |\\_)___/\)/\   
  <__)------(__/   
```
Define a multi-line string array representing the logo.
Include the logo lines in the output if specified by the mask.

### Resource Management
Ensure that resources like allocated memory, file descriptors, or major/minor numbers are released when the module is removed.
This can be handled in the kfetch_mod_exit function.

### Optional Color Support
Implementing color support would involve adding ANSI color codes to your output strings based on certain conditions or user settings.

### Example Implementation
Here's a simplified outline of how some parts of the code might look:

``` C
static int device_open(struct inode *inode, struct file *file) {
    // Check if device is open; if not, set Device_Open and initialize
}

static int device_release(struct inode *inode, struct file *file) {
    // Reset Device_Open and perform cleanup
}

static ssize_t device_read(struct file *file, char __user *user_buffer, size_t count, loff_t *position) {
    // Return data based on mask, including logo and system info
}

static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *position) {
    // Update mask based on user input
}

static void kfetch_mod_exit(void) {
    // Perform resource cleanup here
}
```
### Demo Example


| ![image](https://github.com/CodeStone1125/Kernel_Module/assets/72511296/6a039a0a-7212-44ff-9a0a-a3ed407918a0) |
|:-----------------------------------:|
| **Demo Example Memory & CPU** |


| ![image](https://github.com/CodeStone1125/Kernel_Module/assets/72511296/988dc484-06d7-4748-a87f-48b5dafdf4a7) |
|:-----------------------------------:|
| **Demo Example Init** |
