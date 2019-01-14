class CSystems;
class CSystems
{
public:
	void KnifeBot(struct usercmd_s *cmd);
	void AutoStrafe(struct usercmd_s *cmd, float frametime);
	void StrafeEmulator(void);
	void BunnyHop(struct usercmd_s *cmd);
	void Scroll(struct usercmd_s *cmd);
	void SimulateMouse(int cyka);
	void GroundStrafe(struct usercmd_s *cmd, float frametime);
	void JumpBug(float frametime, struct usercmd_s *cmd);
	//void StrafeHelper(float frametime, struct usercmd_s *cmd);
#define SCROLLUP 120
#define SCROLLDOWN -120
#define MAX_PATTERNS 12
	int scrollpatterns[MAX_PATTERNS][14] =
	{
		//G+{1,2,3,4,5,6,7,8,9,10,11
			{1,0,1,0,0,1,0,0,0,1,0,1,0,1},
			{0,1,0,0,0,1,0,1,0,0,0,0,0,0},
			{1,0,1,0,0,0,0,0,0,0,1,1,0,1},
			{0,1,0,1,0,0,1,0,0,0,0,0,0,0},
			{1,0,1,0,1,0,0,1,0,1,0,0,0,1},
			{0,0,1,0,1,0,1,0,0,0,1,0,1,0},
			{0,1,0,1,0,1,0,0,0,0,0,0,0,0},
			{0,0,1,0,0,1,1,0,0,1,0,0,1,1},
			{1,0,0,1,0,1,0,0,1,0,0,0,0,0},
			{0,1,0,0,1,0,0,1,0,0,0,0,1,0},
			{0,1,0,0,1,0,1,0,1,0,1,0,0,1},
			{0,0,1,0,1,0,0,0,0,0,0,0,1,0}
	};

};
extern CSystems g_Systems;

