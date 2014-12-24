import javax.imageio.ImageIO;
import javax.swing.*;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.*;
import java.io.File;
import java.io.IOException;
import java.util.Random;

@SuppressWarnings("serial")
public class ProjectTwoGUI extends JFrame {

	JList srcIpList = new JList();
	JList destIpList = new JList();
	protected JButton sendButton = new JButton("Send");
	String src;
	String dest;
	Random rand = new Random();
	int randNum;
	Location[] locations;
	String[] listData;
	protected ColorRectangle rectangle;
	Boolean droppedPacket = new Boolean(false);
	Boolean nextDroppedPacket = new Boolean(false);
	Color packetColor = new Color(0,0,0,255);
	protected JLabel sourceTitle;
	protected JLabel destTitle;

	Location test = new Location();
	
	public static void main(String[] args) {

		new ProjectTwoGUI("CPE400 Project 2");
	}
	
	@SuppressWarnings("unchecked")
	public ProjectTwoGUI(String title) 
	{
		super(title);
		this.setTitle(title);
		setSize(735, 510);
		addWindowListener(new WindowDestroyer());  
		
		listData = new String[10];
		locations = new Location[10];
		rectangle = new ColorRectangle();
		sourceTitle = new JLabel("Source");		
		destTitle = new JLabel("Destination");
		
		createIpAdresses();
		
		srcIpList.setListData(listData);
		destIpList.setListData(listData);

		// add listeners to the required objects
		srcIpList.addListSelectionListener( new ListHandler());
		destIpList.addListSelectionListener( new ListHandler());
		sendButton.addActionListener(new ActionHandler());

		getContentPane().setLayout(null);
		
		// get image and add it to frame
		JLabel lblimage = getImage();
		lblimage.setBounds(0, 0, 500, 500);

		// add all objects to the screen
		getContentPane().add(rectangle);
		getContentPane().add(lblimage);
		getContentPane().add(srcIpList);
		getContentPane().add(destIpList);
		getContentPane().add(sendButton);
		getContentPane().add(sourceTitle);
		getContentPane().add(destTitle);

		// set the locations of all the objects
		srcIpList.setBounds(500, 50, 100, 250);
		destIpList.setBounds(605, 50, 100, 250);
		sendButton.setBounds(560, 325, 70, 30);
		rectangle.setBounds(900, 95, 15, 15);
		sourceTitle.setBounds(500, 20, 100, 40);
		destTitle.setBounds(605, 20, 100, 40);
		
		srcIpList.setSelectedIndex(0);
		destIpList.setSelectedIndex(0);

		setVisible(true);
	}
	
	// this function sets up all of the IP addresses used by each host
	private void createIpAdresses() {
		
		for( int i = 0; i < 7; i++ ) {
			locations[i] = new Location();
			locations[i].firstByte = 223;
			locations[i].secondByte = 1;
			listData[i] = locations[i].firstByte + "." + locations[i].secondByte;
		}
		
		locations[0].thirdByte = 1;
		locations[0].fourthByte = 1;
		listData[0] += "." + locations[0].thirdByte + "." + locations[0].fourthByte;
		locations[0].xCoor = 190;
		locations[0].yCoor = 95;
		
		locations[1].thirdByte = 1;
		locations[1].fourthByte = 2;
		listData[1] += "." + locations[1].thirdByte + "." + locations[1].fourthByte;
		locations[1].xCoor = 258;
		locations[1].yCoor = 95;
		
		locations[2].thirdByte = 1;
		locations[2].fourthByte = 3;
		listData[2] += "." + locations[2].thirdByte + "." + locations[2].fourthByte;
		locations[2].xCoor = 305;
		locations[2].yCoor = 95;
		
		locations[3].thirdByte = 2;
		locations[3].fourthByte = 1;
		listData[3] += "." + locations[3].thirdByte + "." + locations[3].fourthByte;
		locations[3].xCoor = 87;
		locations[3].yCoor = 385;
		
		locations[4].thirdByte = 2;
		locations[4].fourthByte = 2;
		listData[4] += "." + locations[4].thirdByte + "." + locations[4].fourthByte;
		locations[4].xCoor = 162;
		locations[4].yCoor = 385;
		
		locations[5].thirdByte = 3;
		locations[5].fourthByte = 1;
		listData[5] += "." + locations[5].thirdByte + "." + locations[5].fourthByte;
		locations[5].xCoor = 320;
		locations[5].yCoor = 385;
		
		locations[6].thirdByte = 3;
		locations[6].fourthByte = 2;
		listData[6] += "." + locations[6].thirdByte + "." + locations[6].fourthByte;
		locations[6].xCoor = 395;
		locations[6].yCoor = 385;
	}

	// open the image file and return the Image object
	private JLabel getImage() {
		
		Image image = null;
		
		try {
			File imageFile = new File("image.jpg");
	        image = ImageIO.read(imageFile);
		} catch (IOException e) {
			System.out.println("Image not found! Exiting...");
			System.exit(0);
		}

		// Use a label to display the image
		JLabel lblimage = new JLabel(new ImageIcon(image));
		
		return lblimage;
	}
	
	// closes program when the window is closed
	private class WindowDestroyer extends WindowAdapter 
	{      
		public void windowClosing(WindowEvent e) 
		{  
			System.exit(0);  
		}                                                             
	}
	
	// this is the class that will actually move the packet
	// from its location to the destination location when the
	// send button is pressed
	private class ActionHandler implements ActionListener 
	{      
		public void actionPerformed(ActionEvent e)
		{
			Boolean firstTime = new Boolean(true);
			
			if( e.getSource() == sendButton ) {
				
				droppedPacket = packetDropped();
				nextDroppedPacket = droppedPacket;
				
				System.out.println();
				System.out.println("sending from " + src + " to " + dest);
				
				// loop until the packet isn't dropped and it isn't the first iteration
				while( droppedPacket || firstTime ) {
					
					packetColor = Color.black;					
					firstTime = false;
					
					droppedPacket = nextDroppedPacket;
				
					// if the src is not the same as the destionation
					if( src.compareTo(dest) != 0 ) {
						
						int srcIndex = srcIpList.getSelectedIndex();
						int destIndex = destIpList.getSelectedIndex();
						
						// if the src is in the top AS, send it to its destination
						if( srcIndex == 0 || srcIndex == 1 || srcIndex == 2  ) {
							
							if( srcIndex == 0 ) {
								topLeftToCenter();
							}
							else if( srcIndex == 1 ) {
								topToCenter();
							}
							else {
								topRightToCenter();
							}
							
							if( !droppedPacket ) {
								if( destIndex == 0 ) {
									fromTopCenterToLeft();
								}
								else if( destIndex == 1 ) {
									fromTopCenterToMiddle();
								}
								else if( destIndex == 2 ) {
									fromTopCenterToRight();
								}
								else if( destIndex == 3 || destIndex == 4 ) {
									
									toTopRouter();
									fromTopToLeftRouter();
									fromLeftRoutertoCenter();
									
									if( destIndex == 3 ) {
										fromLeftCenterToLeft();
									}
									else {
										fromLeftCenterToRight();
									}
								}
								else {
									toTopRouter();
									fromTopToRightRouter();
									fromRightRoutertoCenter();
									
									if( destIndex == 5 ) {
										fromRightCenterToLeft();
									}
									else {
										fromRightCenterToRight();
									}
								}
							}
							else {
								try {
									System.out.println("Packet dropped! Retransmitting...");
									Thread.currentThread();
									Thread.sleep(500);
								} catch (InterruptedException e2) {
									System.out.println("exception caught");
									continue;
								}
							}
						}
						// else if the src is in the bottom left AS,
						// send it to its corresponding destination
						else if( srcIndex == 3 || srcIndex == 4 ) { 
							
							if( srcIndex == 3 ) {
								bottomLeftToCenter();
							}
							else {
								centerLeftToCenter();
							}
							
							if( !droppedPacket ) {
							
								if( destIndex == 3 ) {
									fromLeftCenterToLeft();
								}
								else if( destIndex == 4 ) {
									fromLeftCenterToRight();
								}
								else if( destIndex == 0 || destIndex == 1 || destIndex == 2 ) {
									
									toLeftRouter();
									fromLeftToTopRouter();
									fromTopRoutertoCenter();
									
									if( destIndex == 0 ) {
										fromTopCenterToLeft();
									}
									else if( destIndex == 1 ) {
										fromTopCenterToMiddle();
									}
									else {
										fromTopCenterToRight();
									}
								}
								else {
									
									toLeftRouter();
									fromLeftToRightRouter();
									fromRightRoutertoCenter();
									
									if( destIndex == 5 ) {
										fromRightCenterToLeft();
									}
									else {
										fromRightCenterToRight();
									}
								}
							}
							else {
								try {
									System.out.println("Packet dropped! Retransmitting...");
									Thread.currentThread();
									Thread.sleep(500);
								} catch (InterruptedException e2) {
									System.out.println("exception caught");
									continue;
								}
							}
						}
						// else the src must be in the bottom right AS
						// and send it to its destination
						else {
							if( srcIndex == 5 ) {
								centerRightToCenter();
							}
							else {
								bottomRightToCenter();
							}
							
							if( !droppedPacket ) {
							
								if( destIndex == 5 ) {
									fromRightCenterToLeft();
								}
								else if( destIndex == 6 ) {
									fromRightCenterToRight();
								}
								else if( destIndex == 0 || destIndex == 1 || destIndex == 2 ) {
									
									toRightRouter();
									fromRightToTopRouter();
									fromTopRoutertoCenter();
									
									if( destIndex == 0 ) {
										fromTopCenterToLeft();
									}
									else if( destIndex == 1 ) {
										fromTopCenterToMiddle();
									}
									else {
										fromTopCenterToRight();
									}
								}
								else {
									
									toRightRouter();
									fromRighttoLeftRouter();
									fromLeftRoutertoCenter();
									
									if( destIndex == 3 ) {
										fromLeftCenterToLeft();
									}
									else {
										fromLeftCenterToRight();
									}
								}
							}
							else {
								try {
									System.out.println("Packet dropped! Retransmitting...");
									Thread.currentThread();
									Thread.sleep(500);
								} catch (InterruptedException e2) {
									 
									System.out.println("exception caught");
									continue;
								}
							}
						}
					}
					// the packet cannot be sent to itself, 
					// so don't send anything
					else {
						System.out.println("Cannot send to self!");
					}
					
					// check to see if the packet will be dropped again
					if( droppedPacket ) {
						nextDroppedPacket = packetDropped();
					}
				}
				
				System.out.println("Packet successfully received!");
			}
		}
	}
	
	// class to listen for a click on the lists of addresses
	private class ListHandler implements ListSelectionListener 
	{    
		public void valueChanged(ListSelectionEvent e)
		{
			if ( e.getSource() == srcIpList )
			{
				src = (String) srcIpList.getSelectedValue();
			}
			else if ( e.getSource() == destIpList )
			{
				dest = (String) destIpList.getSelectedValue();
			}
		}
	}
	
	// this is the class used to hold all the information 
	// pertaining to each end host
	private class Location {
		
		protected int firstByte;
		protected int secondByte;
		protected int thirdByte;
		protected int fourthByte;
		protected int xCoor;
		protected int yCoor;
	}
	
	// creates the packet and colors it black
	public class ColorRectangle extends JComponent {
		
		public void paint(Graphics g)
		{
			Dimension d = getSize();

			g.setColor(packetColor);
			g.fillRect(1, 1, d.width-2, d.height-2);
		}

	}
	
	// calculate drop rate to see if packet will be dropped
	private Boolean packetDropped() {
		
		Boolean dropped = new Boolean(false);
		
		Random rand = new Random();
		int randNum = rand.nextInt() % 3;
		randNum = Math.abs(randNum);
		
		if( randNum == 2 ) {
			dropped = true;
		}
		else {
			dropped = false;
		}
		
		return dropped;
	}
	
/////////////////////////////////////////////////////////////////////////////////////////////
	/* 
	 * All methods below this point deal with moving the packet around the screen.
	 * Each method moves the packet from one spot to another spot individually.
	 */
	private void topLeftToCenter() {
		
		int offset = 5;
		
		for( int i = 0; i < 7; i++ ) {
			rectangle.setBounds(locations[0].xCoor + offset, locations[0].yCoor, 15, 15);
			offset += 7;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
private void topToCenter() {
		
		int offset = 5;
		
		for( int i = 0; i < 4; i++ ) {
			rectangle.setBounds(locations[1].xCoor - offset, locations[1].yCoor, 15, 15);
			offset += 5;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}

	private void topRightToCenter() {
		
		int offset = 8;
		
		for( int i = 0; i < 6; i++ ) {
			rectangle.setBounds(locations[2].xCoor - offset, locations[2].yCoor, 15, 15);
			offset += 11;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void bottomLeftToCenter() {
			
			int offset = 5;
			
			for( int i = 0; i < 8; i++ ) {
				rectangle.setBounds(locations[3].xCoor + offset, locations[3].yCoor, 15, 15);
				offset += 5;
				try {
					Thread.sleep(150);
				} catch (InterruptedException e) {
					 
					e.printStackTrace();
				}
				 
				update(this.getGraphics());
			}
		}
	
	private void centerLeftToCenter() {
		
		int offset = 2;
		
		for( int i = 0; i < 8; i++ ) {
			rectangle.setBounds(locations[4].xCoor - offset, locations[4].yCoor, 15, 15);
			offset += 5;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void centerRightToCenter() {
			
			int offset = 2;
			
			for( int i = 0; i < 8; i++ ) {
				rectangle.setBounds(locations[5].xCoor + offset, locations[5].yCoor, 15, 15);
				offset += 5;
				try {
					Thread.sleep(150);
				} catch (InterruptedException e) {
					 
					e.printStackTrace();
				}
				 
				update(this.getGraphics());
			}
		}
	
	private void bottomRightToCenter() {
		
		int offset = 2;
		
		for( int i = 0; i < 8; i++ ) {
			rectangle.setBounds(locations[6].xCoor - offset, locations[6].yCoor, 15, 15);
			offset += 5;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void toLeftRouter() {

		int offset = 2;
		
		for( int i = 0; i < 6; i++ ) {
			rectangle.setBounds( 128, 385 - offset, 15, 15);
			offset += 10;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void toRightRouter() {

		int offset = 2;
		
		for( int i = 0; i < 6; i++ ) {
			rectangle.setBounds( 355, 385 - offset, 15, 15);
			offset += 11;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void toTopRouter() {

		int offset = 2;
		
		for( int i = 0; i < 8; i++ ) {
			rectangle.setBounds( 238, 94 + offset, 15, 15);
			offset += 10;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromTopToLeftRouter() {

		int offsetX = 2;
		int offsetY = 2;
		
		for( int i = 0; i < 9; i++ ) {
			rectangle.setBounds( 238 - offsetX, 162 + offsetY, 15, 15);
			offsetX += 13;
			offsetY += 19;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromTopToRightRouter() {

		int offsetX = 2;
		int offsetY = 2;
		
		for( int i = 0; i < 9; i++ ) {
			rectangle.setBounds( 238 + offsetX, 162 + offsetY, 15, 15);
			offsetX += 13;
			offsetY += 19;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromLeftToRightRouter() {

		int offsetX = 2;
		
		for( int i = 0; i < 10; i++ ) {
			rectangle.setBounds( 150 + offsetX, 325, 15, 15);
			offsetX += 20;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromLeftToTopRouter() {

		int offsetX = 2;
		int offsetY = 5;
		
		for( int i = 0; i < 9; i++ ) {
			rectangle.setBounds( 140 + offsetX, 325 - offsetY, 15, 15);
			offsetX += 13;
			offsetY += 19;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromRightToTopRouter() {

		int offsetX = 2;
		int offsetY = 5;
		
		for( int i = 0; i < 9; i++ ) {
			rectangle.setBounds( 360 - offsetX, 325 - offsetY, 15, 15);
			offsetX += 13;
			offsetY += 19;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromRighttoLeftRouter() {

		int offsetX = 2;
		
		for( int i = 0; i < 13; i++ ) {
			rectangle.setBounds( 360 - offsetX, 325, 15, 15);
			offsetX += 20;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromLeftRoutertoCenter() {

		int offsetY = 2;
		
		for( int i = 0; i < 6; i++ ) {
			rectangle.setBounds( 126, 325 + offsetY, 15, 15);
			offsetY += 10;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromRightRoutertoCenter() {

		int offsetY = 5;
		
		for( int i = 0; i < 8; i++ ) {
			rectangle.setBounds( 357, 325 + offsetY, 15, 15);
			offsetY += 8;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromTopRoutertoCenter() {

		int offsetY = 5;
		
		for( int i = 0; i < 8; i++ ) {
			rectangle.setBounds( 236, 170 - offsetY, 15, 15);
			offsetY += 9;
			try {
				Thread.sleep(170);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromTopCenterToLeft() {

		int offsetX = 5;
		
		for( int i = 0; i < 8; i++ ) {
			rectangle.setBounds( 236 - offsetX, 95, 15, 15);
			offsetX += 6;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromTopCenterToMiddle() {

		int offsetX = 5;
		
		for( int i = 0; i < 5; i++ ) {
			rectangle.setBounds( 236 + offsetX, 95, 15, 15);
			offsetX += 4;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromTopCenterToRight() {

		int offsetX = 5;
		
		for( int i = 0; i < 7; i++ ) {
			rectangle.setBounds( 236 + offsetX, 95, 15, 15);
			offsetX += 10;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromLeftCenterToLeft() {

		int offsetX = 4;
		
		for( int i = 0; i < 5; i++ ) {
			rectangle.setBounds( 130 - offsetX, 385, 15, 15);
			offsetX += 9;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromLeftCenterToRight() {

		int offsetX = 8;
		
		for( int i = 0; i < 4; i++ ) {
			rectangle.setBounds( 130 + offsetX, 385, 15, 15);
			offsetX += 7;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromRightCenterToRight() {

		int offsetX = 5;
		
		for( int i = 0; i < 5; i++ ) {
			rectangle.setBounds( 352 + offsetX, 385, 15, 15);
			offsetX += 8;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
	
	private void fromRightCenterToLeft() {

		int offsetX = 6;
		
		for( int i = 0; i < 7; i++ ) {
			rectangle.setBounds( 352 - offsetX, 385, 15, 15);
			offsetX += 5;
			try {
				Thread.sleep(150);
			} catch (InterruptedException e) {
				 
				e.printStackTrace();
			}
			 
			update(this.getGraphics());
		}
	}
}
	
	
	
	
	
	
	
	
	
	
