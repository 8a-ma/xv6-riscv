#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device

#define M_PROTECT 5   // Protect Mode
#define M_READ    1   // Read mode
#define M_WRITE   2   // Write Mode
#define M_ALL     3   // All mode 

struct stat {
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short type;  // Type of file
  short nlink; // Number of links to file
  uint64 size; // Size of file in bytes

  uint mode;
};
