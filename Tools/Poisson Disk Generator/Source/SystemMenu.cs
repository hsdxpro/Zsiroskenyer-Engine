using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace PoissonDiskGen
{
    // System menu helper class by Florian Stinglmayr
    // http://www.codeguru.com/csharp/csharp/cs_misc/userinterface/article.php/c9327/

    public class NoSystemMenuException : System.Exception
    {
    }

    // Values taken from MSDN.
    public enum ItemFlags
    { // The item ...
       mfUnchecked    = 0x00000000,    // ... is not checked
       mfString       = 0x00000000,    // ... contains a string as label
       mfDisabled     = 0x00000002,    // ... is disabled
       mfGrayed       = 0x00000001,    // ... is grayed
       mfChecked      = 0x00000008,    // ... is checked
       mfPopup        = 0x00000010,    // ... Is a popup menu. Pass the
                                       //     menu handle of the popup
                                       //     menu into the ID parameter.
       mfBarBreak     = 0x00000020,    // ... is a bar break
       mfBreak        = 0x00000040,    // ... is a break
       mfByPosition   = 0x00000400,    // ... is identified by the position
       mfByCommand    = 0x00000000,    // ... is identified by its ID
       mfSeparator    = 0x00000800     // ... is a seperator (String and
                                       //     ID parameters are ignored).
    }

    public enum WindowMessages
    {
        wmSysCommand = 0x0112
    }

    public class SystemMenu
    {
        [DllImport("USER32", EntryPoint = "GetSystemMenu", SetLastError = true,
                   CharSet = CharSet.Unicode, ExactSpelling = true,
                   CallingConvention = CallingConvention.Winapi)]
        private static extern IntPtr win32GetSystemMenu(IntPtr WindowHandle, int bReset);

        [DllImport("USER32", EntryPoint = "AppendMenuW", SetLastError = true,
                   CharSet = CharSet.Unicode, ExactSpelling = true,
                   CallingConvention = CallingConvention.Winapi)]
        private static extern int win32AppendMenu(IntPtr MenuHandle, int Flags, int NewID, String Item);

        [DllImport("USER32", EntryPoint = "InsertMenuW", SetLastError = true,
                   CharSet = CharSet.Unicode, ExactSpelling = true,
                   CallingConvention = CallingConvention.Winapi)]
        private static extern int win32InsertMenu(IntPtr hMenu, int Position, int Flags, int NewId, String Item);

        private IntPtr SysMenu = IntPtr.Zero;

        // Retrieves a new object from a Form object
        static public SystemMenu CreateFromForm(Form f)
        {
            SystemMenu menu = new SystemMenu();
            menu.SysMenu = win32GetSystemMenu(f.Handle, 0);
            if (menu.SysMenu == IntPtr.Zero)
                throw new NoSystemMenuException();

            return menu;
        }

        // Insert a separator at the given position index, starting at zero
        public bool InsertSeparator(int Pos)
        {
            return (InsertMenu(Pos, ItemFlags.mfSeparator | ItemFlags.mfByPosition, 0, ""));
        }

        // Simplified InsertMenu(), that assumes that Pos is a relative
        // position index starting at zero
        public bool InsertMenu(int Pos, int ID, String Item)
        {
            return (InsertMenu(Pos, ItemFlags.mfByPosition | ItemFlags.mfString, ID, Item));
        }

        // Insert a menu at the given position. The value of the position
        // depends on the value of Flags. See the article for a detailed
        // description.
        public bool InsertMenu(int Pos, ItemFlags Flags, int ID, String Item)
        {
            return (win32InsertMenu(SysMenu, Pos, (Int32)Flags, ID, Item) == 0);
        }

        // Appends a seperator
        public bool AppendSeparator()
        {
            return AppendMenu(0, "", ItemFlags.mfSeparator);
        }

        // This uses the ItemFlags.mfString as default value
        public bool AppendMenu(int ID, String Item)
        {
            return AppendMenu(ID, Item, ItemFlags.mfString);
        }

        public bool AppendMenu(int ID, String Item, ItemFlags Flags)
        {
            return (win32AppendMenu(SysMenu, (int)Flags, ID, Item) == 0);
        }

        // Resets the window menu to its default
        public static void ResetSystemMenu(Form f)
        {
            win32GetSystemMenu(f.Handle, 1);
        }

        // Checks if an ID for a new system menu item is OK or not
        public static bool VerifyItemID(int ID)
        {
            return (bool)(ID < 0xF000 && ID > 0);
        }
    }
}
