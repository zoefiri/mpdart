# mpdart
![mpdart] (/mpart.png)

A small c program I made for displaying album art for mpd. Not all mpd clients support displaying album art and some simply can't due to having text based interfaces which motivated me to make this small program.

## usage
### start the daemon by providing the following flags:
`--host [mpd host address]`
`--port [mpd host port]`
`--dir [mpd music dir]`
### display album art with the daemon running by executing with no arguments.


## deps
- libmpdclient
- mpd
- feh
- ffmpeg
- pkg-config

## building
`make`
`make install`


