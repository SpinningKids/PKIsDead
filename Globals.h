
inline int WIDTH = 640;
inline int HEIGHT = 480;
inline int BITSPERPIXEL = 32;
constexpr char WINDOWTITLE[] = "SK Intro";

//Undefine FULLSCREEN if windowed

//what the hell means the line above ????
//Undefine WHITE if BLACK ???  ;D  (rIO)

#ifdef _DEBUG
constexpr bool FULLSCREEN = false;
#else
constexpr bool FULLSCREEN = true;
#endif
