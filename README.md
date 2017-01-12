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
* sc68 for Atari and Amiga SC68 and sndh files
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

* uade module playback
* other retro formats
* ogg, etc

Why?
====
I started working on a retro music player for BB10 devcies. The coded started as a very simple test
on using libplayfp and QNX QSA audio output. Then it grew. And became a messy tangle of stuff bolted on
and adding new formats and features started to be way too complicated.

So I started to create a simple framework for playback of various retro formats that works on plain Qt / QNX / BlackBerry 10.
