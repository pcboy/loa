## Loa

Loa is a webkit based browser mostly (if not fully) keyboard controlled. The idea is reducing features (to keep it fast and bugless), and maximizing the home row use for faster browsing. For now, it's optimized for a Colemak keymap (but of course it will be configurable soon).

### How does it work ?

Main keys:

- s: Stop page loading.
- h: previous page
- n: next page
- i: Insert mode: You can't enter text in command mode. You must be in insert mode (like in vim).
- o: Open mode: Enter any url (don't forget the protocol field (e.g [http://)](http://))) and loa will open it.
- u: Url mode: enter red shortcut for easy url keyboard access.
- d: Search mode: enter search term, do it again for the next one.
- esc: Go back to command mode.

Don't worry, I will explain. First, loa is modal, like vim. So, the first time you start loa, you are in command mode (abbreviated 'Cmd' in the status bar). If you want to insert text, you must be in insert mode (i.e you must type 'i'). If you want to access an url (say [http://bitbucket.org](http://bitbucket.org)) you must go back in command mode (with esc), and pass to Open mode (with 'o'), type [http://bitbucket.org](http://bitbucket.org), and enter. The first time you started loa there must be something that puzzled you; What the fuck is these red letters on rendered pages in url mode !? In fact, loa generates quick access shortcuts for each url. Look at this screenshot:

[![Google search for 東京渋谷 with loa](./img/loa\_screen.png)](./img/loa\_screen.png)

Google search for 東京渋谷 with loa (click to enlarge)

As you can see, there is some red text for each url in the page. If you type 'u' (for url mode), and enter for instance 'tt' loa will open the url with the girl on the down right of the page. You will master it very quickly, it's pretty efficient.

### FAQ

- Why loa ?
  - Loa is a group of voodoo gods in the Neuromancer written by William Gibson. Also I wanted something short. You can also note that 「loa」is composed of more than 66% of home row letters (on colemak/qwerty) for fast typing!

- It sucks, there are no tabs ?
  - You are right, it sucks. Tabs will happen soon when I will have time. For now, just open more loa instances.

- Which Platforms are supported ?
  - I've only tested it on Linux. So I dunno.

- Sometimes, it segfaults !
  - Yeah I know. Not me. Webkit. Check with gdb if you don't believe me. Feel free to send patchs to webkit-gtk developers.

### Dependencies

See DEPENDENCIES file in the build directory. But mostly webkit-gtk.

### License

Loa is under the WTFPL (Do What The Fuck You Want To Public License).

