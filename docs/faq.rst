==========================
Frequently Asked Questions
==========================

The best way to get answers to your questions is to post them on the `Angband forum`_.

.. contents:: Contents
   :local:

Issues and problems
-------------------

How do I report a bug?
~~~~~~~~~~~~~~~~~~~~~~

Post on the `Angband forum`_.

Bug reports should include:

* your current operating system (e.g. Windows 10)
* what version the problem appeared in
* the best steps you can figure out to reproduce the bug.

Savefiles that show the problem might be requested, because they help tracking bugs down.

Is there a way to disable that thing that pops up when you hit the enter key?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Go into the options menu, choose "Edit keymaps", then "Create a keymap".  Press Enter at the "Key" prompt and a single space as the "Action".

And then you'll probably want to choose to "Append keymaps to a file" so that it persists for next time you load the game.

This just replaces the default action of Enter with a "do nothing but don't tell me about help" action. If you want to keep the menu available, say on the 'Tab' key, you can also remap the Tab keypress to the ``\n`` action.


Development
-----------

What are the current plans for the game?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It was chiefly written to form the codebase for the in-development Beleriand_. The best place for discussion is the `Sil section`_ of the forums.

How do I suggest an idea/feature?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Post it on the forums.  If people think it's a good idea, it will generally get some discussion; if they don't, it won't.

How do I get a copy of the source code?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Go to the GitHub_ page, where you can find the bleeding edge as well as all previous versions.

How do I compile the game?
~~~~~~~~~~~~~~~~~~~~~~~~~~

Please see the compiling section of the manual.

How do I contribute to the game?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You have two options:

* Write your patch and submit it as a pull request on GitHub.
* Post about it on the forum.

All contributions are accepted as dual-licenced with both the Angband and GPLv2 licences.

There are contribution guidelines in CONTRIBUTING.md in the top level directory of the source code.

If the patch is a bugfix, then you can reasonably expect it to be integrated into the development tree. If it's more involved, and the feature is not one that the next version is planned to have, the patch may go through several reviews before being incorporated. It may also just be unsuitable for Angband - in which case, please don't take rejection badly; you may just be better off writing a variant.

Non-code activities are different. Documentation can be written on the wiki, or if you're a graphics designer (and they're always welcome) then please talk on the mailing list about your work.


.. _GitHub: https://github.com/NickMcConnell/NarSil/
.. _Beleriand: https://github.com/NickMcConnell/Beleriand/
.. _Angband forum: http://angband.live/forums
.. _Sil section: http://angband.live/forums/forum/angband/sil
