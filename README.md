CuteMedia, a framework test
===========================
A test framework to playback various audio formats using only features of Qt plus
format specific libraries.

It consists of a couple of components: sources, sinks, scanner, decoder and player.

Sources
=======
Audio sources are QIODevice based so they can be read and written to like any Qt IO source.

Currently implemented are:

* libsidplayfp for SID files
* libmodplug for Tracker modules
* libasap for SAP files
* FLAC

Basic usage of source goes like this:
* The source file data is loaded normally into a buffer, say a QByteArray
* source is opened for writing and the data is written to it
* source is closed
* source is opened for reading
* source is assigned to a sink, for example a QAudioOutput
* playback!

Sinks
=====
Currently work in progress, test app is using QAudioOutput

* Qt QAudioOutput
* ALSA
* QNX QSA
* Raw data file writer
* WAV file writer

Scanner
=======
Media file scanner, for now using only file extensions to decide what source to use.

Decoder
=======
Helper to check what source to use. Works.

Player
======
To make playback easy as:
* load();
* play/stop/pause();

Works.

TODO
====
Support is planned for more formats:

* sc68
* uade module playback
* ogg, etc

Why?
====
I need a simple framework to playback various retro formats that works on Qt / QNX / BlackBerry 10.
