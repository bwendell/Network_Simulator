#include "WeightedGraph.cpp"
#include "show12.cpp"
#include <string> 
#include <fstream>
#include <cstdlib>

using namespace std;

void displayTitle();

void displayMenu();

void readInData( WeightedGraph [], WeightedGraph::Vertex , ifstream & );

void intializeNetwork( WeightedGraph [] );

void updateNetwork( WeightedGraph [] );

void updataEBGP( WeightedGraph [] );

void addRouterToAS( WeightedGraph [] );

void networkInfo( WeightedGraph [] );

void getInfoOnRouter( WeightedGraph [] );

void getInfoOnAS( WeightedGraph[] );

bool findRouter( WeightedGraph [], string, int &, int & );

bool getSourceDest( WeightedGraph [], int &, int &, int &, int &, int &, int & );

void displayNetwork( WeightedGraph network[] );

void sendData( WeightedGraph [], int, int, int, int, int, int );

void sendFrame( string, string, string, string, string, string, char, int );

void sendACK( string, string, string, string, string, string, char, int );

bool packetError( int );

int main(){

	// random number seed
	srand( time( NULL ) );

	int sourceHost;
	int destHost;
	int sourceASIndex = 0;
	int sourceRouterIndex = 0;
	int destASIndex = 0;
	int destRouterIndex = 0;
	
	int menuSelection = 0;
	
	ifstream fin;
	fin.clear();
	fin.open( "data.txt" );
	
	// create the network of ASes
	WeightedGraph network[ 10 ];
	// temp router used to add routers into AS 
	WeightedGraph::Vertex router;
	
	readInData( network, router, fin );
	intializeNetwork( network );
				
	// main program loop   
	do{
		
		displayTitle();
		cin >> menuSelection;
		cout << endl;
		
		switch( menuSelection ) {
		
			case 1:
			
				displayNetwork( network );
				break;
				
			case 2:
				
				networkInfo( network );
				break;
			
			case 3:
			
				updateNetwork( network );
				break;
			
			case 4:

				getSourceDest( network, sourceHost, destHost, sourceASIndex, 
							   sourceRouterIndex, destASIndex, destRouterIndex );			   
	
				sendData( network, sourceHost, destHost, sourceASIndex, 
						  sourceRouterIndex, destASIndex, destRouterIndex );
				
				break;
				
			case 5:
				
				cout << "toodles~ (◍•ᴗ•◍)" << endl;
				break;
				
			default:
			
				cout << "ERROR: INVALID INPUT!" << endl;
		}
		
		
	}while( menuSelection != 5 );
			  
	return 0;
}

void displayTitle() {
	
	cout << "-------------------------------------------------------------------" << endl;
	cout << "╔╗ ╔═╗╦═╗╔╦╗╔═╗╦═╗  ╔═╗╔═╗╔╦╗╔═╗╦ ╦╔═╗╦ ╦  ╔═╗╦═╗╔═╗╔╦╗╔═╗╔═╗╔═╗╦  " << endl;
	cout << "╠╩╗║ ║╠╦╝ ║║║╣ ╠╦╝  ║ ╦╠═╣ ║ ║╣ ║║║╠═╣╚╦╝  ╠═╝╠╦╝║ ║ ║ ║ ║║  ║ ║║  " << endl;
	cout << "╚═╝╚═╝╩╚══╩╝╚═╝╩╚═  ╚═╝╩ ╩ ╩ ╚═╝╚╩╝╩ ╩ ╩   ╩  ╩╚═╚═╝ ╩ ╚═╝╚═╝╚═╝╩═╝" << endl;
	cout << "-------------------------------------------------------------------" << endl << endl;
	
	cout << "[ 1 ] DISPLAY NETWORK" << endl;
	cout << "[ 2 ] NETWORK INFORMATION" << endl;
	cout << "[ 3 ] UPDATE NETWORK" << endl;
	cout << "[ 4 ] SEND DATAGRAM" << endl;
	cout << "[ 5 ] QUIT" << endl << endl;
	
	cout << "PLEASE MAKE A SELECTION: ";
}


/* this function is used to set up the network. it reads in the routers
 * along with their prefix & BGP attributes & creates a network
 */
void readInData( WeightedGraph network[], WeightedGraph::Vertex router, ifstream &fin ) {
	
	string temp;
	
	for( int i = 0; i < 3; i++ ) {
		
		network[ i ].bgpIndex = 0;
		
		fin >> network[ i ].ASnumber;
		fin >> network[ i ].ipPrefix;
		
		router.bgpSession = true;

		for( int j = 0; j < 3; j++ ) {
			
			fin >> temp;
			router.setLabel( temp );
			
			fin >> router.attributes.weight;
			fin >> router.attributes.localPreference;
			fin >> router.attributes.MED;
			fin >> router.attributes.origin;
			fin >> router.attributes.community;
		
			for( int k = 0; k < 4; k++ ) {
				
				fin >> router.subnet[ k ];
			}
			
			router.numberOfHosts = 4;
			network[ i ].insertVertex( router );
			router.bgpSession = false;
		}
	}
	
	fin.close();
}

/* this function intializes the network & creates the links 
 * between the routers contained within each AS within the network
 */
void intializeNetwork( WeightedGraph network[] ) {
	
	network[ 0 ].insertEdge( "135.168.1.0", "135.168.2.0", 10 );
	network[ 0 ].insertEdge( "135.168.1.0", "135.168.3.0", 50 );
	network[ 0 ].insertEdge( "135.168.2.0", "135.168.3.0", 20 );

	network[ 1 ].insertEdge( "192.101.1.0", "192.101.2.0", 30 );
	network[ 1 ].insertEdge( "192.101.1.0", "192.101.3.0", 10 );
	network[ 1 ].insertEdge( "192.101.2.0", "192.101.3.0", 30 );
	
	network[ 2 ].insertEdge( "235.155.1.0", "235.155.2.0", 20 );
	network[ 2 ].insertEdge( "235.155.1.0", "235.155.3.0", 10 );
	network[ 2 ].insertEdge( "235.155.2.0", "235.155.3.0", 5 );
}

/* this function allows you to get information about the network
 * this include information about an AS or about a router within the AS
 */
void networkInfo( WeightedGraph network[] ) {
	
	int menuSelection;
	
	do {
		cout << "[ NETWORK INFORMATION ] " << endl << endl;
		cout << "[ 1 ] AS INFORMATION" << endl;
		cout << "[ 2 ] ROUTER INFORMATION" << endl;
		cout << "[ 3 ] MAIN MENU" << endl;
		cout << "\nPLEASE MAKE A SELECTION: ";
		cin >> menuSelection;
		
		if( menuSelection == 1 ) {
			
			getInfoOnAS( network );
		}
		else if( menuSelection == 2 ) {
			
			getInfoOnRouter( network );
		}
		else{ 
			
			cout << "ERROR: INVALID INPUT!" << endl;
		}
		
	}while( menuSelection != 3 );
}

/* this function gets information about a router contained within an AS
 * within the network. it displays the BGP attributes for the router 
 * which are used to determine the weight of the path within the AS.
 */
void getInfoOnRouter( WeightedGraph network[] ) {
	
	int ASnumber;
	int routerNumber;
	info attributes;
	
	cout << endl;
	
	for( int i = 0; i < 3; i++ ) {
		
		cout << "[ " << i + 1 << " ] - AS #" << network[ i ].ASnumber << endl;
	}	
	
	cout << "\nPLEASE SELECT AN AS: ";
	cin >> ASnumber;
	
	cout << "\nROUTER LIST : " << endl;
	
    for ( int i = 0; i < network[ ASnumber -1 ].size; i++ ) {
		
		cout << i << '\t' << network[ ASnumber - 1 ].vertexList[ i ].getLabel() << '\t' << endl;
	}
	
	cout << "\nPLEASE SELECT A ROUTER: ";
	cin >> routerNumber;
	
	attributes = network[ ASnumber - 1 ].vertexList[ routerNumber ].attributes;
	
	cout << "\nROUTER INFO" << endl << endl;
	
	cout << "WEIGHT: " << attributes.weight << endl << endl;
	cout << "[ADDITIONAL INFO] Weight is an attribute that is local to a router. \nThe weight attribute is not advertised to neighboring routers. If the \nrouter learns about more than one route to the same destination, the \nroute with the highest weight will be preferred." << endl;
	cout << endl;
	cout << "LOCAL PREFERENCE: " << attributes.localPreference << endl << endl;
	cout << "[ADDITIONAL INFO] The local preference attribute is used to prefer an \nexit point from the local autonomous system (AS). Unlike the weight \nattribute, the local preference attribute is propagated throughout the \nlocal AS. If there are multiple exit points from the AS, the local \npreference attribute is used to select the exit point for a specific route. " << endl;
	cout << endl;
	cout << "MULTI-EXIT DISCRIMINATOR: " << attributes.MED << endl << endl;
	cout << "[ADDITIONAL INFO] The multi-exit discriminator (MED) or metric attribute \nis used as a suggestion to an external AS regarding the preferred route \ninto the AS that is advertising the metric." << endl;
	cout << endl;
	cout << "ORIGIN: " << attributes.origin << endl << endl;
	cout << "[ADDITIONAL INFO] The origin attribute indicates how BGP learned about a \nparticular route. The origin attribute can have one of three possible values: " << endl << endl;
	cout << "\tIGP - The route is interior to the originating AS. This value is set \n\twhen the network router configuration command is used to inject the \n\troute into BGP." << endl << endl;
	cout << "\tEGP - The route is learned via the Exterior Border Gateway Protocol." << endl << endl;
	cout << "\tIncomplete - The origin of the route is unknown or learned in some \n\tother way. An origin of incomplete occurs when a route is redistributed \n\tinto BGP." << endl << endl;
	cout << "COMMUNITY: " << attributes.community << endl << endl;	
	cout << "[ADDITIONAL INFO] The community attribute provides a way of grouping \ndestinations, called communities, to which routing decisions (such as \nacceptance, preference, and redistribution) can be applied." << endl << endl;
	cout << "\tno-export - Do not advertise this route to EBGP peers." << endl << endl;
	cout << "\tno-advertise - Do not advertise this route to any peer. " << endl << endl;
	cout << "\tinternet - Advertise this route to the Internet community; \n\tall routers in the network belong to it. " << endl << endl;
}

/* this funciton displays information about an AS. the information includes
 * the routers contained within the AS, the links between the routers &
 * the corresponding weight, as well as the shortest path that could be
 * taken from one router to another within the AS. also the router that
 * has the eBGP link which is used to communicate with other ASes is 
 * marked.
 */
void getInfoOnAS( WeightedGraph network[] ) {
	
	int ASnumber;
	
	cout << endl;
	
	for( int i = 0; i < 3; i++ ) {
		
		cout << "[ " << i + 1 << " ] - AS #" << network[ i ].ASnumber
		     << " - IP PREFIX: " << network[ i ].ipPrefix << endl;
	}
	
	cout << "\nPLEASE SELECT AN AS: ";
	cin >> ASnumber;
	
	network[ ASnumber - 1 ].showStructure();
	network[ ASnumber - 1 ].showShortestPaths();
	
	cout << endl;
}

/* this funciton displays the network.  within the network are three ASes
 * within each AS there are three routers, within each router is a subnet
 * with four hosts.
 */
void displayNetwork( WeightedGraph network[] ) {
	
	WeightedGraph::Vertex router;
	
	cout << "[ DISPLAY NETWORK ] " << endl << endl;
	
	for( int i = 0; i < 3; i++ ) {
		
		cout << "----------------------------------------------------------" << endl;
		cout << "[ AS ] - " << i + 1 << "\n[ IP PREFIX: " << network[ i ].ipPrefix << " ]" << endl << endl;
		
		for( int j = 0; j < network[ i ].size; j++ ) {
		
			network[ i ].retrieveVertex( network[ i ].vertexList[ j ].getLabel(), router );
			cout << "\t[ ROUTER ] - " << j + 1 << " - IP PREFIX: " << router.getLabel()
			     << "/24";
			
			if( router.bgpSession ) {
				
				cout << " [ eBGP LINK ]";
			}
			
			cout << endl << endl;
			
			cout << "\t\t[ SUBNET ] " << endl;
	
			for( int i = 0; i < router.numberOfHosts; i++ ) {
				
				cout << "\t\t" << router.subnet[ i ] << endl;
			}	
			
			cout << endl;
		}
		
		cout << "----------------------------------------------------------" << endl;
	}
}

/* this function looks for a router within the network. if the router exists 
 * within the AS it returns which AS it belongs to & which router it is
 * if it is not found then it returns false.
 */
bool findRouter( WeightedGraph network[], string router, int &asIndex, int &routerIndex ) {
	
	for( int i = 0; i < 3; i++ ) {
		
		for( int j = 0; j < network[ i ].size; j++ ) {
			
			if( network[ i ].vertexList[ j ].getLabel() == router ) {
				
				asIndex = i;
				routerIndex = j;
				return true;
			}
		}
	}
	
	return false;
}

/* this function allows you to update the eBGP link of an AS. an eBGP
 * link is used to communicate with other ASes & this function allows you
 * you to update which router within an AS will be the one that 
 * communicates with other eBGP links.
 */
void updataEBGP( WeightedGraph network[] ) {
	
	int ASnumber;
	int routerNumber;
	
	for( int i = 0; i < 3; i++ ) {
		
		cout << "[ " << i + 1 << " ] - AS #" << network[ i ].ASnumber << endl;
	}	
	
	cout << "\nPLEASE SELECT AN AS: ";
	cin >> ASnumber;
	
	cout << "\nCURRENT eBGP ROUTER LINK: " << network[ ASnumber - 1 ].vertexList[ network[ ASnumber - 1 ].bgpIndex ].getLabel() << endl;
	
	cout << "\nROUTER LIST : " << endl;
	
    for ( int i = 0; i < network[ ASnumber -1 ].size; i++ ) {
		
		cout << i + 1 << '\t' << network[ ASnumber - 1 ].vertexList[ i ].getLabel() << '\t' << endl;
	}
	
	cout << "\nPLEASE SELECT A ROUTER: ";
	cin >> routerNumber;
	
	network[ ASnumber - 1 ].vertexList[ network[ ASnumber - 1 ].bgpIndex ].bgpSession = false;
	network[ ASnumber - 1 ].vertexList[ routerNumber - 1 ].bgpSession = true;
	network[ ASnumber - 1 ].bgpIndex = routerNumber - 1;
	
	cout << "\nNEW eBGP ROUTER LINK:" << network[ ASnumber - 1 ].vertexList[ network[ ASnumber - 1 ].bgpIndex ].getLabel() << endl << endl;
}

/* this funcion allows you to "start a new company" or add a new router
 * to an existing AS.  the function allows you to pick an IP address & 
 * by using IP prefixing the company is inserted into the AS.  the function
 * also requires that you submit the BGP attributes for your company
 * which will be used to deteremine the weight of the router within the
 * AS.  the function also lets you pick which router you want to link with
 * in the AS.  the function allows you to make this new router the new
 * eBGP link within in the network.
 */
void addRouterToAS( WeightedGraph network[] ) {
	
	int ASnumber;
	string routerName;
	string nameCheck;
	bool routerNameSelected;
	WeightedGraph::Vertex router;
	
	cout << endl;
	
	for( int i = 0; i < 3; i++ ) {
		
		cout << "[ " << i + 1 << " ] - AS #" << network[ i ].ASnumber << endl;
	}	
	
	cout << "\nPLEASE SELECT AN AS: ";
	cin >> ASnumber;
	
	do{
		
		routerNameSelected = true;
		
		cout << "\nCURRENT ROUTER LIST\n" << endl;
		
		for ( int i = 0; i < network[ ASnumber -1 ].size; i++ ) {
			
			cout << i + 1 << '\t' << network[ ASnumber - 1 ].vertexList[ i ].getLabel() << '\t' << endl;
		}		
		
		cout << "\nPLEASE ENTER ROUTER NAME( ex: 135.168.3.0 or 235.155.2.0 ) \nMUST CONTAIN THE IP PREFIX: ";
		
		for( int i = 0; i < 7; i++ ) {
			
			cout << network[ ASnumber - 1 ].ipPrefix[ i ]; 
		}
		
		cout << endl;
		
		cin >> routerName;
		
		for( int i = 0; i < network[ ASnumber - 1 ].size; i++ )
		{
			if( network[ ASnumber - 1 ].vertexList[ i ].getLabel() == routerName )
			{
				cout << "\nERROR: ROUTER ALREADY IN AS" << endl;
				routerNameSelected = false;
				break;
			}
		}
		
		nameCheck = network[ ASnumber - 1 ].vertexList[ 0 ].getLabel();
		
		for( int i = 0; i < 7; i++ ) {
			
			if( nameCheck[ i ] != routerName[ i ] ) {
				
				cout << "\nERROR: IP PREFIX DOES NOT MATCH AS" << endl;
				routerNameSelected = false;
				break;
			}
		}
		
		if( routerName[ 10 ] != '0' ) {
			
			cout << "\nERROR: THIS IP PREFIX USED FOR SUBNET" << endl;
			routerNameSelected = false;
		}
		
	}while( !routerNameSelected );
	
	router.setLabel( routerName );
	
	string subnetName = routerName;
	
	subnetName[ 10 ] = '1';
	router.subnet[ 0 ] = subnetName;
	
	subnetName[ 10 ] = '2';
	router.subnet[ 1 ] = subnetName;
	
	subnetName[ 10 ] = '3';
	router.subnet[ 2 ] = subnetName;
	
	subnetName[ 10 ] = '4';
	router.subnet[ 3 ] = subnetName;		
	
	router.numberOfHosts = 4;
	
	cout << "\n[ BGP ATTRIBUTES ]" << endl;
	
	cout << "\nPLEASE ENTER WEIGHT: ";
	cin >> router.attributes.weight;
	
	cout << "\nPLEASE ENTER LOCAL PREFERENCE: ";
	cin >> router.attributes.localPreference;

	cout << "\nPLEASE ENTER MED: ";
	cin >> router.attributes.MED;
	
	cout << "\nORIGIN: " << endl;
	cout << "IBGP" << endl;
	cout << "EBGP" << endl;
	cout << "Incomplete" << endl;
	
	cout << "\nPLEASE ENTER ORIGIN: " << endl;
	cin >> router.attributes.origin;
			
	cout << "\nCOMMUNITY: " << endl;
	cout << "no-export" << endl;
	cout << "no-advertise" << endl;
	cout << "Internet" << endl;
	
	cout << "\nPLEASE ENTER COMMUNITY: " << endl;
	cin >> router.attributes.community;
	
	cout << "\nROUTER LIST\n" << endl;
	
	for ( int i = 0; i < network[ ASnumber -1 ].size; i++ ) {
		
		cout << i + 1 << '\t' << network[ ASnumber - 1 ].vertexList[ i ].getLabel() << '\t' << endl;
	}
	
	network[ ASnumber - 1 ].insertVertex( router );			
	
	int routerEdge;
	
	cout << "\nSELECT ROUTER TO CREATE LINK WITH:" << endl;
	cin >> routerEdge;

	network[ ASnumber - 1 ].insertEdge( network[ ASnumber - 1 ].vertexList[ routerEdge - 1 ].getLabel(), routerName, 10 );

	cout << "\nCREATING LINK WITH ROUTER" << endl;
	
	cin.ignore();
	
	cout << "\nPROPAGATING REACHABILITY INFORMATION TO ALL INTERNAL-AS ROUTERS" << endl;
	
	cin.ignore();

	int bgpUpdate;
	
	cout << "[ UPDATE eBGP ]" << endl;
	cout << "\n[ 1 ] MAKE THIS ROUTER THE NEW eBGP LINK FOR AS" << endl;
	cout << "\n[ 2 ] RETURN TO UPDATE NETWORK" << endl;
	
	cout << "\nPLEASE MAKE A SELECTION: ";
	cin >> bgpUpdate;
	
	if( bgpUpdate == 1 ) {
		
		cout << "\nUPDATING eBGP REACHABILITY" << endl;
		network[ ASnumber - 1 ].vertexList[ network[ ASnumber - 1 ].bgpIndex ].bgpSession = false;
		network[ ASnumber - 1 ].vertexList[ network[ ASnumber - 1 ].size - 1 ].bgpSession = true;
		network[ ASnumber - 1 ].bgpIndex = network[ ASnumber - 1 ].size - 1;
	}
}

/* this function displays the menu that is used to update the network
 * you have two options: update eBGP link or add router to as
 */
void updateNetwork( WeightedGraph network[] ) {
	
	int menuSelection;
	
	do {
		
		cout << "\n[ UPDATE NETWORK ] " << endl << endl;
		
		cout << "[ 1 ] UPDATE eBGP LINK" << endl;
		cout << "[ 2 ] ADD ROUTER TO AS" << endl;
		cout << "[ 3 ] MAIN MENU" << endl;
		
		cout << "\nPLEASE MAKE A SELECTION: ";
		cin >> menuSelection;
		
		if( menuSelection == 1 ) {
			
			updataEBGP( network );
		}
		else if( menuSelection == 2 ) {
			
			addRouterToAS( network );
		}
		
	}while( menuSelection != 3 );
}

/* this function is used to when sending data.  the function requires
 * that you pick a router within an AS & a host from the subnet & then
 * you pick a router from another AS & a host from that subnet.  this
 * is essentially picking the source host & a dest host which will send
 * & receive data via the BGP through the ASes.  
 */
bool getSourceDest( WeightedGraph network[], int &sourceHost, int &destHost, int &sourceASIndex, 
					 int &sourceRouterIndex, int &destASIndex, int &destRouterIndex ) {
	
	WeightedGraph::Vertex router;
	string sourceRouter;
	string destRouter;
	bool sourceRouterSelected;
	bool destRouterSelected;
	
	displayNetwork( network );
	
	do {
	
		cout << "PLEASE SELECT A SOURCE ROUTER( ex: 135.168.2.0 or 235.155.1.0 ) "<< endl;
		cin >> sourceRouter;
	
		sourceRouterSelected = findRouter( network, sourceRouter, sourceASIndex, sourceRouterIndex );
		
		if( !sourceRouterSelected ) {
			
			cout << "\nERROR: ROUTER NOT IN AS PLEASE SELECT AGAIN" << endl;
		}	
	
		if( network[ sourceASIndex ].vertexList[ sourceRouterIndex ].bgpSession ) {
			
			cout << "[ ATTENTION ] - eBGP ROUTER SELECTED!" << endl;
			cout << "PLEASE SELECT ANOTHER ROUTER FOR SIMULATION PURPOSES\n" << endl;
			sourceRouterSelected = false;
		}
		
	}while( !sourceRouterSelected );
	
	
	if( sourceRouterSelected ) {
		
		network[ sourceASIndex ].retrieveVertex( network[ sourceASIndex ].vertexList[ sourceRouterIndex ].getLabel(), router );
		
		for( int i = 0; i < router.numberOfHosts; i++ ) {
			
			cout << "[ " << i + 1 << " ]" << router.subnet[ i ] << endl;
		}			
		
		cout << "\nPLEASE SELECT A HOST:";
		cin >> sourceHost;
		sourceHost--;
		
		cout << "\nPLEASE SELECT A DESTINATION ROUTER( ex: 135.168.2.0 or 235.155.1.0 ) "<< endl;
		cin >> destRouter;
		
		do {
		
			cout << "\nPLEASE SELECT A DESTINATION ROUTER( ex: 135.168.2.0 or 235.155.1.0 ) "<< endl;
			cin >> destRouter;
		
			destRouterSelected = findRouter( network, destRouter, destASIndex, destRouterIndex );

			if( !destRouterSelected ) {
				
				cout << "\nERROR: ROUTER NOT IN AS PLEASE SELECT AGAIN" << endl;
			}
		
			if( network[ destASIndex ].vertexList[ destRouterIndex ].bgpSession ) {
				
				cout << "[ ATTENTION ] - eBGP ROUTER SELECTED!" << endl;
				cout << "PLEASE SELECT ANOTHER ROUTER FOR SIMULATION PURPOSES\n" << endl;
				destRouterSelected = false;
			}
			
		}while( !destRouterSelected );		
		
		if( destRouterSelected ) {
			
			network[ destASIndex ].retrieveVertex( network[ destASIndex ].vertexList[ destRouterIndex ].getLabel(), router );
			
			for( int i = 0; i < router.numberOfHosts; i++ ) {
				
				cout << "[ " << i + 1 << " ]" << router.subnet[ i ] << endl;
			}			
			
			cout << "\nPLEASE SELECT A HOST:";
			cin >> destHost;
			destHost--;
			
			cout << "\nOBTAINING SUBNET REACHABILITY FROM NEIGHBORING AS" << endl;
			return true;
		}
		else
		{
			cout << "\nROUTER NOT IN NETWORK!" << endl;
		}	
	}
	else
	{
		cout << "\nROUTER NOT IN NETWORK!" << endl;
	}
	
	return false;
}

void clearScreen() {
	
	cout << string( 40, '\n' );
}

/* this function is used to print to the screen & simulate data 
 * traveling through the network from source to dest.  do not worry too
 * much about the code it is mostly for printing, it shows the source host
 * & then the router which the source host forwards the data to & then 
 * that router forwards data to the eBGP router which will in turn 
 * forward the data to the corresponding ASes eBGP & so forth
 */
void sendFrame( string one, string two, string three, string four, string five, string six, char data, int frame ) {

	bool packetTransmitted = false;
	
	cin.ignore();
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
	cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
		 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
	cout << "[   FRAME #" << frame << "  ]" << endl;
	cout << "[   SENDING   ]" << endl;
	cin.ignore();
	
	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t    [     LOST    ]" << endl;
			cout << "\t \t    [  RETRANSMIT ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "[   FRAME #" << frame << "  ]" << endl;
			cout << "[   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t    [   FRAME #" << frame << "  ]" << endl;
			cout << "\t \t    [   RECEIVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );
	
	packetTransmitted = false;
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
		 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
	cout << "\t \t    [   FRAME #" << frame << "  ]" << endl;
	cout << "\t \t    [   SENDING   ]" << endl;
	cin.ignore();
	
	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t[     LOST    ]" << endl;
			cout << "\t \t \t \t \t[  RETRANSMIT ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t    [   FRAME #" << frame << "  ]" << endl;
			cout << "\t \t    [   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t[   FRAME #" << frame << "  ]" << endl;
			cout << "\t \t \t \t \t[   RECEIVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );

	packetTransmitted = false;
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
		 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
	cout << "\t \t \t \t \t[   FRAME #" << frame << "  ]" << endl;
	cout << "\t \t \t \t \t[   SENDING   ]" << endl;
	cin.ignore();		

	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t    [     LOST    ]" << endl;
			cout << "\t \t \t \t \t \t \t    [  RETRANSMIT ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t[   FRAME #" << frame << "  ]" << endl;
			cout << "\t \t \t \t \t[   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t    [   FRAME #" << frame << "  ]" << endl;
			cout << "\t \t \t \t \t \t \t    [   RECEIVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );
	
	packetTransmitted = false;
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
		 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
	cout << "\t \t \t \t \t \t \t    [   FRAME #" << frame << "  ]" << endl;
	cout << "\t \t \t \t \t \t \t    [   SENDING   ]" << endl;
	cin.ignore();
	
	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[     LOST    ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[  RETRANSMIT ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t    [   FRAME #" << frame << "  ]" << endl;
			cout << "\t \t \t \t \t \t \t    [   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[   FRAME #" << frame << "  ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[   RECEIVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );		
	
	packetTransmitted = false;
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
		 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
	cout << "\t \t \t \t \t \t \t \t \t \t[   FRAME #" << frame << "  ]" << endl;
	cout << "\t \t \t \t \t \t \t \t \t \t[   SENDING   ]" << endl;
	cin.ignore();	
	
	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t \t \t    [     LOST    ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t \t \t    [  RETRANSMIT ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[   FRAME #" << frame << "  ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
				 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t \t \t    [   FRAME #" << frame << "  ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t \t \t    [   RECEIVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO CONTINUE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] --> [ " << two << " ] --> [ " << three << " ] --> [ "
		 << four << " ] --> [ " << five << " ] --> [ " << six << " ]" << endl;
	cout << "\t \t \t \t \t \t \t \t \t \t \t \t    [   FRAME #" << frame << "  ]" << endl;
	cout << "[ ATTENTION ] - FRAME #" << frame << " SENT FROM: " << one 
	     << " HAS BEEN RECEIVED AT: " << six << " WITH THE DATA: " << data 
	     << "  [   RECEIVED  ]" << endl;
	cout << "[ ATTENTION ] - ACK #" << frame << " WILL BE SENT FROM: " << six 
	     << " TO BE RECEIVED AT: " << one << endl;
	cin.ignore();
}

/* this function is used to print to the screen & simulate ack 
 * traveling through the network from dest to source.  do not worry too
 * much about the code it is mostly for printing, it shows the dest host
 * & then the router which the dest host forwards the data to & then 
 * that router forwards data to the eBGP router which will in turn 
 * forward the ack to the corresponding ASes eBGP & so forth
 */
void sendACK( string one, string two, string three, string four, string five, string six, char data, int frame ) {

	bool packetTransmitted = false;
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
		 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
	cout << "\t \t \t \t \t \t \t \t \t \t \t \t    [    ACK #" << frame << "   ]" << endl;
	cout << "\t \t \t \t \t \t \t \t \t \t \t \t    [   SENDING   ]" << endl;
	cin.ignore();
	
	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[     LOST    ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[  RETRANSMIT ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t \t \t    [    ACK #" << frame << "   ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t \t \t    [   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[    ACK #" << frame << "   ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[  RECEIEVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );
	
	packetTransmitted = false;
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
		 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[    ACK #" << frame << "   ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[   SENDING   ]" << endl;
	cin.ignore();
	
	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t    [     LOST    ]" << endl;
			cout << "\t \t \t \t \t \t \t    [  RETRANSMIT ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[    ACK #" << frame << "   ]" << endl;
			cout << "\t \t \t \t \t \t \t \t \t \t[   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t    [    ACK #" << frame << "   ]" << endl;
			cout << "\t \t \t \t \t \t \t    [   RECEIVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );

	packetTransmitted = false;
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
		 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
	cout << "\t \t \t \t \t \t \t    [    ACK #" << frame << "   ]" << endl;
	cout << "\t \t \t \t \t \t \t    [   SENDING   ]" << endl;
	cin.ignore();		

	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t[    LOST     ]" << endl;
			cout << "\t \t \t \t \t[ RETRANSMIT  ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t \t \t    [    ACK #" << frame << "   ]" << endl;
			cout << "\t \t \t \t \t \t \t    [   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t[    ACK #" << frame << "   ]" << endl;
			cout << "\t \t \t \t \t[  RECEIEVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );
	
	packetTransmitted = false;
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
		 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
	cout << "\t \t \t \t \t[    ACK #" << frame << "   ]" << endl;
	cout << "\t \t \t \t \t[   SENDING   ]" << endl;
	cin.ignore();
	
	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t    [     LOST    ]" << endl;
			cout << "\t \t    [  RETRANSMIT ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t \t \t \t[    ACK #" << frame << "   ]" << endl;
			cout << "\t \t \t \t \t[   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t    [    ACK #" << frame << "   ]" << endl;
			cout << "\t \t    [   RECEIVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );		
	
	packetTransmitted = false;
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
		 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
	cout << "\t \t    [    ACK #" << frame << "   ]" << endl;
	cout << "\t \t    [   SENDING   ]" << endl;
	cin.ignore();	
	
	do {
		
		if( packetError( 80 ) ) {
			
			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "[     LOST    ]" << endl;
			cout << "[  RETRANSMIT ]" << endl;
			cin.ignore();

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "\t \t    [    ACK #" << frame << "   ]" << endl;
			cout << "\t \t    [   SENDING   ]" << endl;
			cin.ignore();				
		}
		else {

			clearScreen();
			cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
			cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
			cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
				 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
			cout << "[    ACK #" << frame << "   ]" << endl;
			cout << "[   RECEIVED  ]" << endl;
			packetTransmitted = true;
			cin.ignore();	
		}
	
	}while( !packetTransmitted );
	
	clearScreen();
	cout << "\t \t \t \t \t \t     [ ROUTE ]" << endl << endl;
			cout << "\t \t \t \t \t    [ PRESS ENTER TO SIMULATE ]" << endl << endl;
	cout << "[ SOURCE HOST ]" << "     [ SOURCE iBGP ]     [ SOURCE eBGP ]     [  DEST eBGP  ]     [  DEST iBGP  ]     [  DEST HOST  ] " << endl;
	cout << "[ " << one << " ] <-- [ " << two << " ] <-- [ " << three << " ] <-- [ "
		 << four << " ] <-- [ " << five << " ] <-- [ " << six << " ]" << endl;
	cout << "[    ACK #" << frame << "   ]" << endl;
	cout << "[   RECEIVED  ] - ACK #" << frame << " SENT FROM: " << six 
	     << " HAS BEEN RECEIVED AT: " << one << endl;
	
	cin.ignore();
}

/* this function is used to send data through the network.  a message
 * is broken down into a number of packets which are send into the network
 * & acks are send back for each packet. this funciton continues until
 * the transmission is completed
 */
void sendData( WeightedGraph network[], int sourceHost, int destHost, int sourceASIndex, 
				int sourceRouterIndex, int destASIndex, int destRouterIndex ) {
	
	string message = "CPE400";
	string messageReceived;
	
	string sourceHostLabel = network[ sourceASIndex ].vertexList[ sourceRouterIndex ].subnet[ sourceHost ];
	string destHostLabel = network[ destASIndex ].vertexList[ destRouterIndex ].subnet[ destHost ];
	
	string sourceRouterLabel = network[ sourceASIndex ].vertexList[ sourceRouterIndex ].getLabel();
	string destRouterLabel = network[ destASIndex ].vertexList[ destRouterIndex ].getLabel();
	
	string sourceBGPlink = network[ sourceASIndex ].vertexList[ network[ sourceASIndex ].bgpIndex ].getLabel();
	string destBGPlink = network[ destASIndex ].vertexList[ network[ destASIndex ].bgpIndex ].getLabel();
	
	unsigned int sizeOfMessage = message.size();
	unsigned int index = 0;
	
	cout << "THE MESSAGE: \"" << message << "\" HAS BEEN BROKEN INTO: " 
	     << sizeOfMessage << " PACKETS THAT WILL BE TRANSMITTED" << endl; 	
	     
	cin.ignore();
	
	cout << "SOURCE HOST: " << sourceHostLabel 
	     << " STARTING A TCP CONNECTION WITH DESTINATION HOST: " 
	     << destHostLabel << endl;
	     
	cin.ignore();       
	
	cout << "TCP CONNECTION IS NOW OPEN" << endl;
	
	cin.ignore();

	while( index < sizeOfMessage ) {
		
		cout << "PREPARING TO SEND [ PACKET #" << index + 1 << " - DATA: ( " << message[ index ] << " ) ]" << endl;
		
		cin.ignore();
		
		sendFrame( sourceHostLabel, sourceRouterLabel, sourceBGPlink, 
				   destBGPlink, destRouterLabel, destHostLabel, message[ index ], ( index + 1 ) );
		
		messageReceived += message[ index ];
		cout << "DATA RECIEVED: " << messageReceived << endl;
		
		cin.ignore();
			   
		sendACK( sourceHostLabel, sourceRouterLabel, sourceBGPlink, 
				 destBGPlink, destRouterLabel, destHostLabel, message[ index ], ( index + 1 ) );
		
		index++;
	}
	
	cout << "\nTRANSMISSION COMPLETE!" << endl;
	
	cin.ignore();
}

/* this function generates packet error which is used for TCP 
 * the errorRate varies to simulate a real network.  depending on the
 * current errorRate, if the packetError is within the errorRate then
 * the packet will be lost, otherwise the packet is good to go
 */
bool packetError( int error ) {

	double packetError;
	double errorRate;
		
	errorRate = ( ( double )( rand() % error + 1) / 100 );
	usleep( 1000 );
	packetError = ( rand() / ( double ) RAND_MAX );
	
	return ( packetError < errorRate );
}
