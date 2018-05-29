# MeasureNetworkTraffic
Count the rate at which UDP packets are received in C

There are a lot of tools out there for measuring how much traffic is coming through a socket. I bet you won't find many as lightweight as MeasureNetworkTraffic! 

To call it, type: `./MeasureNetworkTraffic 127.0.0.1 5000` It'll bind a socket to that address, and then show you the rate at which data is accumulated at that port. By default it refreshes every second. Requires the [ncurses library](https://www.gnu.org/software/ncurses/) and [Supersocket](https://github.com/dbrandman/SuperSocket). To get even more accurate performance, run it using the PREMPT_RT kernel using and use chrt.
