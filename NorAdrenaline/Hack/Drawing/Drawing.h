class CDrawing;
class CDrawing
{
public:
	void DrawRect(int x, int y, int w, int h, int r, int g, int b, int a);
	void DrawOutlinedRect(int x, int y, int w, int h, int r, int g, int b, int a);
	void DrawCircle(float position[2], float points, float radius, int r, int g, int b, int a);
	void DrawCircle3D(Vector position, float points, float radius, int r, int g, int b, int a);
	void DrawLine(int x0, int y0, int x1, int y1, int r, int g, int b, int a);
	void DrawString(vgui::HFont font, int x, int y, int r, int g, int b, int a, DWORD alignment, const char* msg, ...);
	void DrawStringACP(vgui::HFont font, int x, int y, int r, int g, int b, int a, DWORD alignment, const char* msg, ...);
	void DrawPlayerBox(int x, int y, int w, int h, int r, int g, int b, int a);
	void DrawBox(int x, int y, int w, int h, int r, int g, int b, int a);
	void BoxOutline(float x, float y, float w, float h, float lw, BYTE r, BYTE g, BYTE b, BYTE a);
	void Box(int x, int y, int w, int h, int lw, int r, int g, int b, int a);
	void DrawTexture(int id, int x0, int y0, int x1, int y1, int r = 255, int g = 255, int b = 255, int a = 255);
	void DrawCoalBox(int x, int y, int w, int h, int lw, int r, int g, int b, int a);
	void DrawOutlineCoalBox(int x, int y, int w, int h, int  lw, BYTE r, BYTE g, BYTE b, BYTE a);
	void SetupFonts();
	void DrawCrosshair(int x, int y, int r, int g, int b, int a);
	void DrawDot(int x, int y, int r, int g, int b, int a);
	void FillArea(int x, int y, int w, int h, int r, int g, int b, int a);
	void BoxCorner(int x, int y, int w, int h, int lw, BYTE r, BYTE g, BYTE b, BYTE a);
	void BoxCornerOutline(int x, int y, int w, int h, int lw, BYTE r, BYTE g, BYTE b, BYTE a);
};
extern CDrawing g_Drawing;

extern vgui::HFont ESP;
extern vgui::HFont MENU;