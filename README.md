# Packet Sniffer C++

This project aims to be an attempt at building out a packet sniffer similar to wireshark in c++.

This will not have nearly as many features, but it will improve my understanding. 

The main libraries involved will be pcap


<img width="7906" height="4856" alt="image" src="https://github.com/user-attachments/assets/e475f6da-3c3d-4c16-a0d0-2e5afc9b4d3d" />

At it's core, a packet sniffer intercepts messages and analyzes their contents. Some of the common usecases that I have found so far include
- Disecting headers
- Analyzing request payloads
- Determining the nature of a network request/what protocol version it uses
  - HTTP/1.1
  - UDP messaging
