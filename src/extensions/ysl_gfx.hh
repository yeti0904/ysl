#ifndef YSL_EXTENSION_GFX_HH
#define YSL_EXTENSION_GFX_HH

#include "../environment.hh"
#include "../util.hh"
#include <raylib.h>

#define ARGSTYPE const std::vector <std::string>&
#define ENVTYPE  Environment&

namespace YSL {
	namespace Extensions {
		namespace Gfx {
			Color ColourFromInt(int col) {
				return {
					(unsigned char) (((col & 4) / 4) * 255),
					(unsigned char) (((col & 2) / 2) * 255),
					(unsigned char) ((col & 1) * 255),
					255
				};
			}
		
			std::vector <int> Init(ARGSTYPE args, ENVTYPE env) {
				env.Assert(args.size() == 3, "Gfx.Init: Needs 3 arguments");
				env.Assert(
					Util::IsInteger(args[0]) && Util::IsInteger(args[1]),
					"Gfx.Init: First 2 arguments must be integers"
				);

				SetTraceLogLevel(LOG_NONE);
				InitWindow(stoi(args[0]), stoi(args[1]), args[2].c_str());
				SetTargetFPS(60);
				BeginDrawing();
				return {};
			}

			std::vector <int> Stop(ARGSTYPE, ENVTYPE) {
				CloseWindow();
				return {};
			}

			std::vector <int> WindowOpen(ARGSTYPE, ENVTYPE) {
				return {WindowShouldClose()? 0 : 1};
			}

			std::vector <int> Draw(ARGSTYPE, ENVTYPE) {
				EndDrawing();
				BeginDrawing();
				return {};
			}

			std::vector <int> Pixel(ARGSTYPE args, ENVTYPE env) {
				env.Assert(args.size() == 3, "Gfx.Pixel: Needs 3 arguments");
				env.Assert(
					Util::IsInteger(args[0]) && Util::IsInteger(args[1]) &&
					Util::IsInteger(args[2]),
					"Gfx.Pixel: Arguments must be integers"
				);

				DrawPixel(stoi(args[0]), stoi(args[1]), ColourFromInt(stoi(args[2])));
			
				return {};
			}
			std::vector <int> Line(ARGSTYPE args, ENVTYPE env) {
				env.Assert(args.size() == 5, "Gfx.Line: Needs 5 arguments");
				env.Assert(
					Util::IsInteger(args[0]) && Util::IsInteger(args[1]) &&
					Util::IsInteger(args[2]) && Util::IsInteger(args[3]) &&
					Util::IsInteger(args[4]),
					"Gfx.Pixel: First 4 arguments must be integers"
				);

				DrawLine(
					stoi(args[0]), stoi(args[1]), stoi(args[2]), stoi(args[3]),
					ColourFromInt(stoi(args[4]))
				);
				
				return {};
			}
			std::vector <int> Rectangle(ARGSTYPE args, ENVTYPE env) {
				env.Assert(args.size() == 5, "Gfx.Rectangle: Needs 5 arguments");
				env.Assert(
					Util::IsInteger(args[0]) && Util::IsInteger(args[1]) &&
					Util::IsInteger(args[2]) && Util::IsInteger(args[3]) &&
					Util::IsInteger(args[4]),
					"Gfx.Rectangle: First 4 arguments must be integers"
				);

				DrawRectangleLines(
					stoi(args[0]), stoi(args[1]), stoi(args[2]), stoi(args[3]),
					ColourFromInt(stoi(args[4]))
				);
				
				return {};
			}

			std::vector <int> FillRectangle(ARGSTYPE args, ENVTYPE env) {
				env.Assert(args.size() == 5, "Gfx.FillRectangle: Needs 5 arguments");
				env.Assert(
					Util::IsInteger(args[0]) && Util::IsInteger(args[1]) &&
					Util::IsInteger(args[2]) && Util::IsInteger(args[3]) &&
					Util::IsInteger(args[4]),
					"Gfx.FillRectangle: First 4 arguments must be integers"
				);

				DrawRectangle(
					stoi(args[0]), stoi(args[1]), stoi(args[2]), stoi(args[3]),
					ColourFromInt(stoi(args[4]))
				);
				
				return {};
			}

			std::vector <int> Clear(ARGSTYPE args, ENVTYPE env) {
				env.Assert(args.size() == 1, "Gfx.Clear: Needs 1 argument");
				env.Assert(Util::IsInteger(args[0]), "Gfx.Clear: Argument must be integer");

				ClearBackground(ColourFromInt(stoi(args[0])));

				return {};
			}

			Extension BuildExtension() {
				Extension ext;

				ext.name                        = "gfx";
				ext.functions["init"]           = Init;
				ext.functions["stop"]           = Stop;
				ext.functions["window_open"]    = WindowOpen;
				ext.functions["draw"]           = Draw;
				ext.functions["pixel"]          = Pixel;
				ext.functions["line"]           = Line;
				ext.functions["rectangle"]      = Rectangle;
				ext.functions["fill_rectangle"] = FillRectangle;
				ext.functions["clear"]          = Clear;

				ext.variables["black"]  = {0};
				ext.variables["red"]    = {0b0100};
				ext.variables["green"]  = {0b0010};
				ext.variables["blue"]   = {0b0001};
				ext.variables["yellow"] = {0b0110};
				ext.variables["cyan"]   = {0b0011};
				ext.variables["white"]  = {0b0111};

				return ext;
			}
		}
	}
}

#endif
