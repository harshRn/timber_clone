#include<sstream>
#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<vector>


using namespace sf;

//Function declaration
void updateBranches(int seed);

const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

//Where is the player/branch?
//Left or Right
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];
//side branchPositions[] = { side::LEFT,side::RIGHT,side::RIGHT,side::NONE,side::LEFT,side::NONE };

int main()
{
	//Create a video mode object
	VideoMode vm(1920, 1080);

	//Create and open a window for the game
	RenderWindow window(vm, "Timber", Style::Fullscreen);

	//Textures
	Texture textureBackground;

	//Load the scene 
	textureBackground.loadFromFile("graphics/background.png");

	//Create A Sprite
	Sprite spriteBackground;

	//Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	//Set the spriteBackgroound to cover the screen
	spriteBackground.setPosition(0, 0);

	//Make a tree Sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	// Trees...more trees
	Texture textureTree2;
	textureTree2.loadFromFile("graphics/tree2.png");
	Sprite spriteTree2;
	spriteTree2.setTexture(textureTree2);
	Sprite spriteTree3;
	spriteTree3.setTexture(textureTree2);
	


	spriteTree2.setPosition(20, 0);
	spriteTree3.setPosition(1500, -100);
	

	// the bee sprite
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	
	//movement
	bool beeActive = false;

	//speed of the bee
	float beeSpeed = 0.0f;

	//making 3 cloud sprite from 1 texture
	Texture textureCloud;

	//Loading the 1st one
	textureCloud.loadFromFile("graphics/cloud.png");

	//3 new sprites
	const int NUM_CLOUDS = 6;
	int c = 0;
	std::vector<Sprite> spriteCloud(NUM_CLOUDS);
	for (int i = 0; i < NUM_CLOUDS; i++, c += 250)
	{
		spriteCloud[i].setTexture(textureCloud);
		spriteCloud[i].setPosition(0, c);
	}
	
	//Are the cloud cuurrently on screen?
	std::vector<bool> cloudActive(NUM_CLOUDS, false);

	//How fast is each cloud	
	std::vector<int>cloudSpeed (NUM_CLOUDS, 0);

	//variables to control the time
	Clock clock;

	//Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidhtPerSecond = timeBarStartWidth / timeRemaining;

	//Track whether the game is running
	bool paused = true;

	//Draw some text
	int score = 0;

	Text messageText;
	Text scoreText;
	Text fpsText;

	//We need to choose a font
	Font font;
	font.loadFromFile("font/KOMIKAP_.ttf");

	//Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);
	fpsText.setFont(font);

	//Assign the actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");

	//Make the text larger
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);
	fpsText.setCharacterSize(100);


	//Color of the text
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);
	fpsText.setFillColor(Color::White);



	//Positioning the text
	FloatRect textRect = messageText.getLocalBounds();

	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);
	fpsText.setPosition(1200, 20);


	//Prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	//Set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++)
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		//Set the sprite's origin to dead centre
		// We can then spin it round without changin its position
		branches[i].setOrigin(220, 20);
	}


	//Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	//The player starts on the left
	side playerSide = side::LEFT;

	//Prepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	//Axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	//Line up the axe with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	//Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	//Some other useful log related variables
	bool logActive = false;
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	//control the player input
	bool acceptInput = false;

	//Prepare the sounds
	//The player chopping sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	//Death
	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	//Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wav");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	//control the text
	int lastDrawn = 0;

	while (window.isOpen())
	{
		/*
		Handle the player's input
		*/

		Event event;

		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				//Listen for key presses again
				acceptInput = true;

				//hide the axe
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}

		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		//Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			std::stringstream ss;
			ss << "Score = " << 0;
			scoreText.setString(ss.str());
			paused = false;

			//reset the time and score
			score = 0;
			timeRemaining = 6;
			
			//Make all the branches disappear
			//starting in the second position
			for (int i = 1; i < NUM_BRANCHES; i++)
			{
				branchPositions[i] = side::NONE;
			}

			// hiding the epitaph
			spriteRIP.setPosition(675, 2000);

			//move the player into position
			spritePlayer.setPosition(580, 720);

			acceptInput = true;
		}

		if (acceptInput)
		{
			//First handle pressing the right cursor key because the default placement is on the left
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				playerSide = side::RIGHT;
				score++;

				//Add to the amount of time remaining
				timeRemaining += (2 / score) + 0.15;

				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);

				//update the branches
				updateBranches(score);

				//set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;
				acceptInput = false;

				//Update the score text
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());


				//Play the chop sound
				chop.play();
			}

			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				//Make sure the player is on the left
				playerSide = side::LEFT;
				score++;

				//Add to the amount of time remianing
				timeRemaining += (2 / score) + 0.15;

				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);

				//update the branches
				updateBranches(score);

				//set the log flying
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;

				//Update the score text
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());

				//Play the chop sound
				chop.play();
			}
		}


		/*
			Scene Update
		*/

		if (!paused)
		{

			//Measure time
			Time dt = clock.restart();

			//Subtract from the amount of time Remaining
			timeRemaining -= dt.asSeconds();
			//size up the time bar
			timeBar.setSize(Vector2f(timeBarWidhtPerSecond * timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f)
			{
				//pause the game
				paused = true;

				//Change the message shown to the player
				messageText.setString("Out of time!!");

				//Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				// Play the out of time sound
				outOfTime.play();
			}

			//Setup the bee
			if (!beeActive)
			{
				//How fast is the bee
				srand((int)time(0));
				beeSpeed = (rand() % 200) + 200;

				//How high is the bee
				srand((int)time(0) * 10);
				float height = (rand() % 500) + 500;
				spriteBee.setPosition(2000, height);
				beeActive = true;
			}

			//Move the bee
			else
			{
				spriteBee.setPosition(
					spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),
					spriteBee.getPosition().y);

				//Has the bee reached the left-hand edge of the screen?
				if (spriteBee.getPosition().x < -100)
				{
					// Set it up ready to be a whole new bee next frame
					beeActive = false;
				}
			}

			// Manage the clouds
			for (int i = 0; i < NUM_CLOUDS; i++)
			{
				if (!cloudActive[i])
				{
					//how fast 

					srand((int)time(0)* (i + 1)*10);
					cloudSpeed[i] = (rand() % 200);

					//How high
					srand((int)time(0) *i );
					float height = (rand() % 150);
					spriteCloud[i].setPosition(-200, height);
					cloudActive[i] = true;
				}
				else
				{
					spriteCloud[i].setPosition(
						spriteCloud[i].getPosition().x + (cloudSpeed[i] * dt.asSeconds()),
						spriteCloud[i].getPosition().y);

					//has the cloud reached the end of the screen
					if (spriteCloud[i].getPosition().x > 1920)
					{
						//Set it up ready to be as a whole new cloud next frame
						cloudActive[i] = false;
					}
				}
			}
			
			lastDrawn++;
			if (lastDrawn == 100) {
				// Update the score text
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());

				// Draw the fps
				std::stringstream ss2;
				ss2 << "FPS = " << 1 / dt.asSeconds();
				fpsText.setString(ss2.str());
				lastDrawn = 0;
			}

			//update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 150;
				if (branchPositions[i] == side::LEFT)
				{
					//Move the sprite to the left side
					branches[i].setPosition(610, height);

					//Flip the sprite round the other way
					branches[i].setRotation(180);
				}

				else if (branchPositions[i] == side::RIGHT)
				{
					//Move the sprite to the right side
					branches[i].setPosition(1330, height);

					//Set the sprite rotation to normal
					branches[i].setRotation(0);
				}
				else
				{
					//hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			//Handle a flying log
			if (logActive)
			{
				spriteLog.setPosition(spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));

				//Has the log reached the right hand edge?
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x>2000)
				{
					//Set it up ready to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			//has the player been squished by a branch?
			if (branchPositions[5] == playerSide)
			{
				//death
				paused = true;
				acceptInput = false;

				//Draw the gravelstone
				spriteRIP.setPosition(525,760);

				//hide the player
				spritePlayer.setPosition(2000, 660);

				//Change the text of the message
				messageText.setString("SQUISHED!!");

				//Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				// Play the death sound
				death.play();
			}
		}



		/*
			Draw
		*/

		window.clear();

		//Draw scene here
		window.draw(spriteBackground);

		//Draw the clouds
		for (int i = 0; i < 3; i++)
			window.draw(spriteCloud[i]);	

		window.draw(spriteTree2);
		window.draw(spriteTree3);
		

		//draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++)
			window.draw(branches[i]);

		//draw the tree
		window.draw(spriteTree);

		//Draw the player
		window.draw(spritePlayer);
		
		//Draw the axe
		window.draw(spriteAxe);

		//Draw the flying log
		window.draw(spriteLog);

		//Draw the gravelstone
		window.draw(spriteRIP);

		//Draw the insect
		window.draw(spriteBee);

		//Draw the score
		window.draw(scoreText);
		window.draw(fpsText);

		//draw the timebar
		window.draw(timeBar);
		if (paused)
		{
			//Draw our message
			window.draw(messageText);
			//window.draw(fpsText);

		}

		//Show everything that's been drawn
		window.display();
	}

	return 0;
}

void updateBranches(int seed)
{
	for (int j = NUM_BRANCHES - 1; j > 0; j--)
	{
		branchPositions[j] = branchPositions[j - 1];
	}

	//Spawn a new branch at position 0
	//LEFT,RIGHT, OR NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r)
	{
	case 0:
		branchPositions[0] = side::LEFT;
		break;

	case 1:
		branchPositions[0] = side::RIGHT;
		break;
	
	default:
		branchPositions[0] = side::NONE;
		break;
	}
}
