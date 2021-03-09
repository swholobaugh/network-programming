//win_init.c
//This program lists all network adapters on a windows system

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

//Network headers
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
//Headers for standard functions/memory allocation
#include <stdio.h>
#include <stdlib.h>

//Define libraries that need to be linked with exe
#pragma comment(lib, "ws2_32.lib");
#pragma comment(lib, "iphlpapi.lib");

int main() {

  WSADATA d;
  if (WSAStartup(MAKEWORD(2, 2), &d)) {
    printf("Failed to initialize.\n");
    return -1;
  }

  //Stores size of adapter's address buffer
  DWORD asize = 20000;
  PIP_ADAPTER_ADDRESSES adapters;
  do {
    //allocate 20,000 bytes to adapters
    adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize);

    if(!adapters) {
      printf("Couldn't allocation %ld bytes for adapters.\n", asize);
      WSACleanup();
      return -1;
    }

    //AF_UNSPEC tells windows to get both IPv4 and IPv6 addresses
    //GAA_FLAG_INCLUDE_PREFIX required to request a list of addresses
    //Reserved parameter that should be passed as 0 or NULL
    //Pass in buffer, adapters and a pointer to its size
    int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0,
				 adapters, &asize);
    
    if (r == ERROR_BUFFER_OVERFLOW) {
      printf("GetAdaptersAddresses wants %ld bytes.\n", asize);
      free(adapters);
    } else if (r == ERROR_SUCCESS) {
      break;
    } else {
      printf("Error from GetAdaptersAddresses: %d\n", r);
      free(adapters);
      WSACleanup();
      return -1;
    }
  } while (!adapters);

  //Use to walk through linked list of adapters
  PIP_ADAPTER_ADDRESSES adapter = adapters;
  while (adapter) {
    //Gets name from adapter
    printf("\nAdapter name: %S\n", adapter->FriendlyName);

    //Set pointer to first address from adapter
    PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;
    //Walks through addresses from each adapter
    while (address) {
      printf("\t%s",
	     address->Address.lpSockaddr->sa_family == AF_INET ?
	     "IPv4" : "IPv6");

      //Character buffer
      char ap[100];

      getnameinfo(address->Address.lpSockaddr,
		  address->Address.iSockaddrLength,
		  ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
      printf("\t%s\n", ap);

      address = address->Next;
    }

    adapter = adapter->Next;
  }

  free(adapters);
  WSACleanup();
  return 0;
}
  
