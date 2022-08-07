#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <time.h>
#include <vector>;

using namespace std;

int RESOLUTION_NES[3] = { 256,240,61440 };
int RESOLUTION_VGA[3] = { 640,480 ,307200};
int RESOLUTION_HD [3] = { 1920,1080, 2073600};
int RESOLUTION_2K[3]  = { 2560,1440,3686400 };

int* RESOLUTION = new int[2];
int* particleMatrix = new int[RESOLUTION[3]];

class ControlInput
{
public:
    SDL_Event event;
    int mousePosition_x = 0;
    int mousePosition_y = 0;
    enum MoveDirection { none = 0, up = 1, down = 2, left = 3, right = 4 };
    enum PlayerAction {idle = 0, attack = 1, ability = 2, action1 = 3, action2 = 4};
    MoveDirection moveDirection = none;
    PlayerAction  playerAction = idle;



    void check()
    {
        moveDirection = none;
        playerAction = idle;

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                // exit game

            }
            else if (event.type == SDL_KEYDOWN)
            {

                switch (event.key.keysym.sym)
                {
                case SDLK_UP:
                    moveDirection = up;
                    cout << "up\n";
                    break;

                case SDLK_DOWN:
                    moveDirection = down;
                    cout << "down\n";
                    break;

                case SDLK_LEFT:
                    moveDirection = left;
                    cout << "LEFT\n";
                    break;

                case SDLK_RIGHT:
                    moveDirection = right;
                    cout << "right\n";
                    break;

                case SDLK_SPACE:
                    playerAction = attack;
                    cout << "Space bar!!\n";
                    break;

                case SDLK_w:
                    cout << "w\n";
                    break;

                case SDLK_s:
                    cout << "s\n";
                    break;

                case SDLK_a:
                    cout << "a\n";
                    break;

                case SDLK_d:
                    cout << "d\n";
                    break;

                default:
                    moveDirection = none;

                }

            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {

                SDL_GetMouseState(&mousePosition_x, &mousePosition_y);
                cout << "Mouse Click: X:" << mousePosition_x << " Y:" << mousePosition_y << endl;

            }
        }
    }

    int getDirection()
    {
        return moveDirection;
    }
};


class SoundEngine
{
public:

    Mix_Chunk* soundFx = NULL;
    Mix_Chunk* playerAttackSound = NULL;
    Mix_Chunk* cantDo = NULL;
    Mix_Music* music = NULL;
    int musicVolume = 128;

    SoundEngine()
    {
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        //Loading game sounds
        playerAttackSound = Mix_LoadWAV("sound/powerUp2.wav");
        soundFx = Mix_LoadWAV("sound/sound.wav");
        cantDo  = Mix_LoadWAV("sound/no-1.wav");

        //Loading game Music
        music = Mix_LoadMUS("music/bgm_action_4.wav");
        

    }

    void quit()
    {
        Mix_FreeChunk(soundFx);
        soundFx = NULL;
        music = NULL;
        playerAttackSound = NULL;
        cantDo = NULL;
    }

    void playSound(string _sound)
    {
        if (_sound == "ding")
        {
            Mix_PlayChannel(-1, soundFx, 0);
        }

        if (_sound == "playerAttack")
        {
            Mix_PlayChannel(-1, playerAttackSound, 0);
        }

        if (_sound == "cantDo")
        {
            Mix_PlayChannel(-1, cantDo, 0);
        }
    }

    void playMusic(string _song)
    {
        Mix_PlayMusic(music, -1);
        Mix_VolumeMusic(musicVolume);
    }

    void musicSetVolume(int _volume)
    {
        musicVolume = _volume;
    }

    void pauseMusic()
    {
        Mix_PauseMusic();
    }

    void unpauseMusic()
    {
        Mix_ResumeMusic();
    }



};


class Particle
{
public:

    int positionX;
    int positionY;
    int particleAge = 0;
    int particleSpeed = 1;
    int type = 0;
    
    int transparency = 255;
    int white[3] = { 255,255,255 };  // snow
    int blue [3] = { 32,32,196};     // water
    int green[3] = { 0,255,0};       // dirt top / grass
    int grey [3] = { 128,128,128 };  // smoke
    int black[3] = { 0,0,0 };
    int red  [3] = { 255, 0, 0 };
    int color[3] = { 0,0,0};

    bool paused = false;

    Particle(SDL_Renderer* _renderer, int _positionX, int _positionY, int _particleSpeed, int _type)
    {
        //Generate a particle
        positionX = _positionX;
        positionY = _positionY;
        particleSpeed = _particleSpeed;
        type = _type;

        if (_type == 1)
        {
            color[0] = white[0];
            color[1] = white[1];
            color[2] = white[2];
        }

        if (_type == 2)
        {
            color[0] = blue[0];
            color[1] = blue[1];
            color[2] = blue[2];
        }
        
    }

    void update(SDL_Renderer* _renderer)
    {
        
        if (paused == false)
        {


            positionY += particleSpeed;
            if (positionY > 480)
            {
                positionX = rand() % 640 + 1;
                positionY = 0;
                particleSpeed = rand() % 5 + 1;
            }

            SDL_SetRenderDrawColor(_renderer, color[0], color[1], color[2], transparency);
            SDL_RenderDrawPoint(_renderer, positionX, positionY);
        }
    }
};


class Stamp
{
public:
    SDL_Rect stampRect;
    SDL_Texture* stampTexture = NULL;
    SDL_Surface* stampSurface;
};


class Background
{
public:
    
    SDL_Surface* wallSurface = IMG_Load("graphics/wall.png");
    SDL_Surface* grassSurface = IMG_Load("graphics/grass.png");
    SDL_Surface* currentBackground = NULL;
    SDL_Texture* backgroundTexture = NULL;
    SDL_Texture* grassTexture = NULL;
    SDL_Texture* wallTexture = NULL; 
    SDL_Renderer* renderer;
    SDL_Rect backgroundRect;

    Background(SDL_Renderer* _renderer)
    { 
        renderer = _renderer;
        backgroundRect.x = 0;
        backgroundRect.y = 0;
        backgroundRect.w = 640;
        backgroundRect.h = 480;
      
    }

    void update()
    {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundRect);
    }

    void load(string _graphic)
    {
        if (_graphic == "wall")
        {
            backgroundTexture = SDL_CreateTextureFromSurface(renderer, wallSurface);
        }

        if (_graphic == "grass")
        {
            backgroundTexture = SDL_CreateTextureFromSurface(renderer, grassSurface);
        }
        // Free the loaded surfaces
        SDL_FreeSurface(currentBackground);
        SDL_FreeSurface(grassSurface);
        SDL_FreeSurface(wallSurface);
    }
};


class Sprite
{
public:
    
    enum MovingDirection {none = 0 , up = 1 , down = 2, left = 3, right = 4};
    enum PlayerAction    { idle = 0, attack = 1, ability = 2, action1 = 3, action2 = 4 };
    MovingDirection movingDirection = none;
    
    //only 1 rect is needed if sprites are all the same size
    SDL_Rect spriteRect;
    
    //create textures for the slime
    SDL_Texture* spriteTexture_up    = NULL;
    SDL_Texture* spriteTexture_down  = NULL;
    SDL_Texture* spriteTexture_left  = NULL;
    SDL_Texture* spriteTexture_right = NULL;
    SDL_Texture* spriteTexture = NULL;

    //int moveDirection = 0 ; 0=pause, 1=up, 2=down, 3=left, 4=right
    int spriteSpeed = 1;
    int spriteSize = 16;
    int tileSize = 16;
    int moveDirection = 0;
    int moveDistance = 64;
    int playerAttackCooldown = 15;
    int playerAttackDelay = 0;
    int max_x = 640;
    int max_y = 480;
    bool playerControl = false;
    bool isMoving = false;
    string spriteType = "";
    string tag = "";
    
    
    Sprite()
    {
        //This constructor is to create an empty sprite for temp data
        spriteRect.x = 0;
        spriteRect.y = 0;
        spriteRect.w = spriteSize;
        spriteRect.h = spriteSize;
        spriteType = "";
    }

    //Create a standardized 16x16 sprite
    Sprite(SDL_Renderer* _renderer, string _spriteType, int _x_position, int _y_position)
    {
        spriteRect.x = _x_position;
        spriteRect.y = _y_position;
        spriteRect.w = spriteSize;
        spriteRect.h = spriteSize;
        spriteType = _spriteType;

        

        

        //create texture from surface for slime
        if (_spriteType == "slime")
        {
            //Load graphics to surface
            SDL_Surface* slimeSurface_down  = IMG_Load("graphics/slime_d.png");
            SDL_Surface* slimeSurface_up    = IMG_Load("graphics/slime_u.png");
            SDL_Surface* slimeSurface_left  = IMG_Load("graphics/slime_l.png");
            SDL_Surface* slimeSurface_right = IMG_Load("graphics/slime_r.png");

            //create texture from surfaces
            spriteTexture_up    = SDL_CreateTextureFromSurface(_renderer, slimeSurface_up);
            spriteTexture_down  = SDL_CreateTextureFromSurface(_renderer, slimeSurface_down);
            spriteTexture_left  = SDL_CreateTextureFromSurface(_renderer, slimeSurface_left);
            spriteTexture_right = SDL_CreateTextureFromSurface(_renderer, slimeSurface_right);

            //free up surfaces
            SDL_FreeSurface(slimeSurface_up);
            SDL_FreeSurface(slimeSurface_down);
            SDL_FreeSurface(slimeSurface_left);
            SDL_FreeSurface(slimeSurface_right);
        }
         
        if (_spriteType == "npc")
        {
            //Load graphics to surface
            SDL_Surface* playerSurface_down  = IMG_Load("graphics/player_d.png");
            SDL_Surface* playerSurface_up    = IMG_Load("graphics/player_u.png");
            SDL_Surface* playerSurface_left  = IMG_Load("graphics/player_l.png");
            SDL_Surface* playerSurface_right = IMG_Load("graphics/player_r.png");

            //create texture from surfaces
            spriteTexture_up    = SDL_CreateTextureFromSurface(_renderer, playerSurface_up);
            spriteTexture_down  = SDL_CreateTextureFromSurface(_renderer, playerSurface_down);
            spriteTexture_left  = SDL_CreateTextureFromSurface(_renderer, playerSurface_left);
            spriteTexture_right = SDL_CreateTextureFromSurface(_renderer, playerSurface_right);

            //free up surfaces
            SDL_FreeSurface(playerSurface_up);
            SDL_FreeSurface(playerSurface_down);
            SDL_FreeSurface(playerSurface_left);
            SDL_FreeSurface(playerSurface_right);
        
            
        }

        if (_spriteType == "projectile")
        {
            //Load graphics to surface
            SDL_Surface* projectileSurface = IMG_Load("projectile.png");

            //create a texture from surface
            spriteTexture = SDL_CreateTextureFromSurface(_renderer, projectileSurface);

            //free up surfaces
            SDL_FreeSurface(projectileSurface);
          


        }
        

    }


    void update(SDL_Renderer* _renderer, ControlInput _controlInput ,SoundEngine _soundEngine)
    {
        
        //update routine for the slime sprite
        if (spriteType == "slime")
        {
            if (isMoving == false)
            {
                moveDirection = rand() % 5;
                moveDistance = rand() % 8 * spriteSize + 1;
                spriteSpeed = rand() % 4 + 1;
                isMoving = true;
            }

            if (moveDirection == 0)
            {
                //standing still
                movingDirection = none;
                moveDistance -= spriteSpeed;


                if (moveDistance <= 0)
                {
                    isMoving = false;
                }

            }

            if (moveDirection == 1)
            {
                //moving up
                movingDirection = up;
                moveDistance -= spriteSpeed;
                spriteRect.y -= spriteSpeed;


                if (spriteRect.y < tileSize)
                {
                    spriteRect.y = tileSize;
                    isMoving = false;
                }
                if (moveDistance <= 0)
                {
                    isMoving = false;
                }
            }

            if (moveDirection == 2)
            {
                // moving down
                movingDirection = down;
                moveDistance -= spriteSpeed;
                spriteRect.y += spriteSpeed;


                if (spriteRect.y > max_y - spriteSize - tileSize)
                {
                    spriteRect.y = max_y - spriteSize - tileSize;
                    isMoving = false;
                }
                if (moveDistance <= 0)
                {
                    isMoving = false;
                }

            }

            if (moveDirection == 3)
            {
                // moving left
                movingDirection = left;
                moveDistance -= spriteSpeed;
                spriteRect.x -= spriteSpeed;


                if (spriteRect.x < tileSize)
                {
                    spriteRect.x = tileSize;
                    isMoving = false;
                }
                if (moveDistance <= 0)
                {
                    isMoving = false;
                }

            }

            if (moveDirection == 4)
            {
                // moving right
                movingDirection = right;
                moveDistance -= spriteSpeed;
                spriteRect.x += spriteSpeed;

                if (spriteRect.x > max_x - spriteSize - tileSize)
                {
                    spriteRect.x = max_x - spriteSize - tileSize;
                    isMoving = false;
                }
                if (moveDistance <= 0)
                {
                    isMoving = false;
                }

            }

        }

        //update routine for the player
        if (spriteType == "npc")
        {            
            
            if (_controlInput.playerAction == attack && playerAttackDelay == 0)
            {
                _soundEngine.playSound("playerAttack");
                playerAttackDelay = playerAttackCooldown;
               
                
            }
            
            moveDirection = _controlInput.getDirection();
            if (isMoving == false)
            {
                
                moveDistance = spriteSize;
                spriteSpeed  = spriteSize;
                isMoving = true;
            }

            if (moveDirection == 0)
            {
                //standing still
                //movingDirection = none;
                moveDistance -= spriteSpeed;


                if (moveDistance <= 0)
                {
                    isMoving = false;
                }

            }

            if (moveDirection == 1)
            {
                //moving up
                movingDirection = up;
                moveDistance -= spriteSpeed;
                spriteRect.y -= spriteSpeed;


                if (spriteRect.y < tileSize)
                {
                    _soundEngine.playSound("cantDo");
                    spriteRect.y = tileSize;
                    isMoving = false;
                }
                if (moveDistance <= 0)
                {
                    isMoving = false;
                }
            }

            if (moveDirection == 2)
            {
                // moving down
                movingDirection = down;
                moveDistance -= spriteSpeed;
                spriteRect.y += spriteSpeed;


                if (spriteRect.y > max_y - spriteSize - tileSize)
                {
                    _soundEngine.playSound("cantDo");
                    spriteRect.y = max_y - spriteSize - tileSize;
                    isMoving = false;
                }
                if (moveDistance <= 0)
                {
                    isMoving = false;
                }

            }

            if (moveDirection == 3)
            {
                // moving left
                movingDirection = left;
                moveDistance -= spriteSpeed;
                spriteRect.x -= spriteSpeed;


                if (spriteRect.x < tileSize)
                {
                    _soundEngine.playSound("cantDo");
                    spriteRect.x = tileSize;
                    isMoving = false;
                }
                if (moveDistance <= 0)
                {
                    isMoving = false;
                }

            }

            if (moveDirection == 4)
            {
                // moving right
                movingDirection = right;
                moveDistance -= spriteSpeed;
                spriteRect.x += spriteSpeed;

                if (spriteRect.x > max_x - spriteSize - tileSize)
                {
                    _soundEngine.playSound("cantDo");
                    spriteRect.x = max_x - spriteSize - tileSize;
                    isMoving = false;
                }
                if (moveDistance <= 0)
                {
                    isMoving = false;
                }

            }

            
            //Attack cooldown for the player
            playerAttackDelay--;
            if (playerAttackDelay < 0) { playerAttackDelay = 0; }
            
        }
 
        // Render sprite to screen at the end of update acording to its direction
        if (movingDirection == down || moveDirection == none)
        {
            SDL_RenderCopy(_renderer, spriteTexture_down, NULL, &spriteRect);
        }

        if (movingDirection == up)
        {
            SDL_RenderCopy(_renderer, spriteTexture_up, NULL, &spriteRect);
        }

        if (movingDirection == left)
        {
            SDL_RenderCopy(_renderer, spriteTexture_left, NULL, &spriteRect);
        }

        if (movingDirection == right)
        {
            SDL_RenderCopy(_renderer, spriteTexture_right, NULL, &spriteRect);
        }
    }

    void onCollision(string _tag)
    {

    }
};


class SpriteBatch
{
public:
    vector <Sprite> * spriteMatrix = new vector<Sprite>;
    Sprite player;
    unsigned int position = 0;
    unsigned int currentUpdatePosition = 0;
    int resolution_x = 640;
    int resolution_y = 480;
    bool paused = false;

    //referances to the needed objects.
    SDL_Renderer* renderer ;
    SoundEngine soundEngine;
   

    SpriteBatch(SDL_Renderer* _renderer, SoundEngine _soundEngine, ControlInput _controlInput)
    {
        
        cout << "SpriteBatch created.\n";
        renderer = _renderer;
        soundEngine = _soundEngine;
        
        
        
    }


    void add(Sprite _sprite)
    {
        //add sprites to the batch
        spriteMatrix->push_back(_sprite);
    }


    void remove(Sprite _sprite)
    {
        // remove a sprite
    }


    void createRandomSprite(string _type, int _count)
    {
 
        for (int x = 0; x < _count; x++)
        {
            Sprite sprite(renderer, _type, resolution_x / 2, resolution_y / 2);
            this->add(sprite);
            
        }
        

    }

    void instantiate(string _type, int _position_x,int position_y)
    {
        //this will create a sprite and add it to the spritebatch
        if (_type == "projectile")
        {
            
        }
    }

    unsigned int updatePosition()
    {
        return currentUpdatePosition;
    }

    void update(ControlInput _controlInput)
    {
        //update sprites in batch
        if (!spriteMatrix->empty() && paused == false)
        {
            //update sprites if not empty
            for (unsigned int x = 0; x < spriteMatrix->size(); x++)
            {
                currentUpdatePosition = x;
                spriteMatrix->at(x).update(renderer,_controlInput,soundEngine);
            }
        }
    }

    void clear()
    {
        //clear all sprites from batch
        spriteMatrix->clear();
    }

    int count()
    {
        //return number of sprites in batch
        return spriteMatrix->size();
    }
};


class Settings
{
public:
    
    bool muteAudio   = false;
    bool useGamepad  = false;
    bool useKeyboard = true;
    bool useMouse    = true;
    int musicVolume = 255;
    int soundVolume = 255;


    //not functional yet , need to use pointer to resolution array;
    int choice = 0;
    bool answered = false;

    Settings()
    {
        cout << "[_____________________________]\n";
        cout << "|   Welcome to my C++ demo    | \n";
        cout << "|         Version .9          |\n";
        cout << "|      Grafx,Sound,Input      |\n";
        cout << "|   Resolution: VGA = 640x480 |\n";
        cout << "|           Jason V           |\n";
        cout << "[_____________________________]\n";
        
        
        choice = 2;
        while (answered == false)
        {

            if (choice == 2)
            {
                RESOLUTION[0] = RESOLUTION_NES[0];
                RESOLUTION[1] = RESOLUTION_NES[1];
                answered = true;
            }

            if (choice == 2)
            {
                RESOLUTION[0] = RESOLUTION_VGA[0];
                RESOLUTION[1] = RESOLUTION_VGA[1];
                answered = true;
            }

            if (choice == 3)
            {
                RESOLUTION[0] = RESOLUTION_HD[0];
                RESOLUTION[1] = RESOLUTION_HD[1];
                answered = true;
            }
        }

    }
};


class SpaceMap
{
public:
    char matrix[307200];
    int position_x = 0;
    int position_y = 0;
    int absolutePosition = 0;

    SpaceMap()
    {
        //constructor
    }

    int getAbsolutePosition(int _x, int _y)
    {
        absolutePosition = _x + _y * 640;
        return absolutePosition;
    }
};


int main(int argc, char** args)
{
   
    //Settings for FPS and constants
    const int FPS = 60;
    const int windowSize_X = 640;//RESOLUTION[0];
    const int windowSize_Y = 480;// RESOLUTION[1];
    const char windowTitle[] = "100 Sprites!";
    Uint32 FRAMETIMESTART;
    bool quit = false;
    

    // create an object to hold program settings
    Settings settings;

    // create a control object to handle any inputs
    ControlInput controlInput;
    
    //Initialize the SDL and IMG systems for use
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    SDL_Init(SDL_INIT_AUDIO);

    // Create a Window and Renderer
    SDL_Window* window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, windowSize_X, windowSize_Y, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    
    //create a sound engine to play audio
    SoundEngine soundEngine;

    // create a sprite batch
    SpriteBatch spriteBatch(renderer,soundEngine,controlInput);
    
    //generate a bunch of slimes and add them to the render batch
    spriteBatch.createRandomSprite("slime", 20);

    //generate a player
    Sprite player(renderer, "npc", 64, 64);
    //add player to render batch
    spriteBatch.add(player);



    //create a background and display wall texture
    Background backGround(renderer);
    backGround.load("wall");
   
    //tell sound engine to to play music named intro and set volume to a lower setting
    soundEngine.musicSetVolume(64);
    soundEngine.playMusic("intro");

    while (!quit)
    {
        FRAMETIMESTART = SDL_GetTicks();
        
        //check for any control inputs.
        controlInput.check();

        // Clear the screen and set the background color.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        //sprite.update(renderer);
        backGround.update();
        spriteBatch.update(controlInput);

        //Copy our updated graphics to the screen;
        SDL_RenderPresent(renderer);

        //FPS Check and maintainer
        if (1000 / FPS > SDL_GetTicks() - FRAMETIMESTART)
        {
            SDL_Delay(1000 / FPS - (SDL_GetTicks() - FRAMETIMESTART));
        }
    }

    //Free the Resources to close game.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    //Quitting systems
    soundEngine.quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}