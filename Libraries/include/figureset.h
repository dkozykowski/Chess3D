#ifndef FIGURES_H
#define FIGURES_H

#include <model.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;


glm::vec3 GetSquareCoord(glm::vec2);

const float BOARD_SCALE = 0.3f;
const float PIECE_SCALE = 0.02f;
const float SQUARE_SIZE = 0.78f;

glm::vec3 STARTING_POS = glm::vec3(3.5f * SQUARE_SIZE, 0, -SQUARE_SIZE);

class Figure : public Model {
public:

    vector<glm::vec2> positionsOnBoard;

    Figure() {}

    Figure(string const& path,
        glm::vec3 position,
        vector<glm::vec2> positionsOnBoard,
        float scale = 1.0f,
        glm::vec3 rotation = glm::vec3(0.0f)) : Model(path, position, scale, rotation) {
        this->positionsOnBoard = positionsOnBoard;
    }

    void Draw(Shader& shader, glm::vec3 rotation = glm::vec3(0)) {
        for (auto positionOnBoard : positionsOnBoard)
            Model::Draw(shader, GetSquareCoord(positionOnBoard), rotation);
    }
};


class Figureset {
public:
	const string pathToModels;

    Figure bishopBlack, kingBlack, pawnBlack, knightBlack, queenBlack, rookBlack;
    Figure bishopWhite, kingWhite, pawnWhite, knightWhite, queenWhite, rookWhite;
    Model board;

    bool FiguresLoaded = false;

	Figureset(string const& path) : pathToModels(path) {
        board = Model(
            pathToModels + "board/board.obj",
            STARTING_POS + glm::vec3(0.0f, 0.0f, 0.0f),
            BOARD_SCALE
        );
	}

    void Draw(Shader& shader) {
        DrawBoard(shader);
        DrawFigures(shader);
    }

	void DrawBoard(Shader& shader) {
        board.Draw(shader);
	}

    void DrawFigures(Shader& shader) {
        if (!FiguresLoaded) return;
        bishopBlack.Draw(shader); 
        bishopBlack.Draw(shader); 
        kingBlack.Draw(shader); 
        pawnBlack.Draw(shader); 
        knightBlack.Draw(shader); 
        queenBlack.Draw(shader); 
        rookBlack.Draw(shader);

        bishopWhite.Draw(shader); 
        bishopWhite.Draw(shader); 
        kingWhite.Draw(shader); 
        pawnWhite.Draw(shader); 
        knightWhite.Draw(shader); 
        queenWhite.Draw(shader); 
        rookWhite.Draw(shader);
    }

	void LoadFigures() {

        if (FiguresLoaded) return;

        FiguresLoaded = true;
        
        // black figures
        bishopBlack = Figure(
            pathToModels + "black/bishop/bishop.obj",
            glm::vec3(0, 0.12f, 3.85f),
            {glm::vec2(5, 0), glm::vec2(2, 0)},
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        kingBlack = Figure(
            pathToModels + "black/king/king.obj",
            glm::vec3(0.0, 0.12f, 5.225f),
            {glm::vec2(4,0)},
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        pawnBlack = Figure(
            pathToModels + "black/pawn/pawn.obj",
            glm::vec3(0, 0.12f, 1.9f),
            { glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(2, 1), glm::vec2(3, 1), 
              glm::vec2(4, 1), glm::vec2(5, 1), glm::vec2(6, 1), glm::vec2(7, 1)},
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        knightBlack = Figure(
            pathToModels + "black/knight/knight.obj",
            glm::vec3(0, 0.12f, 3.2f),
            { glm::vec2(1, 0), glm::vec2(6, 0) },
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        queenBlack = Figure(
            pathToModels + "black/queen/queen.obj",
            glm::vec3(0, 0.12f, 4.52f),
            { glm::vec2(3, 0) },
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        rookBlack = Figure(
            pathToModels + "black/rook/rook.obj",
            glm::vec3(0, 0.12f, 2.55f),
            { glm::vec2(0, 0), glm::vec2(7, 0) },
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );

        // white figures
        bishopWhite = Figure(
            pathToModels + "white/bishop/bishop.obj",
            glm::vec3(0, 0.12f, 3.85f),
            { glm::vec2(5, 7), glm::vec2(2, 7) },
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        kingWhite = Figure(
            pathToModels + "white/king/king.obj",
            glm::vec3(0.0, 0.12f, 5.225f),
            { glm::vec2(4, 7) },
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        pawnWhite = Figure(
            pathToModels + "white/pawn/pawn.obj",
            glm::vec3(0, 0.12f, 1.9f),
            { glm::vec2(0, 6), glm::vec2(1, 6), glm::vec2(2, 6), glm::vec2(3, 6),
              glm::vec2(4, 6), glm::vec2(5, 6), glm::vec2(6, 6), glm::vec2(7, 6) },
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        knightWhite = Figure(
            pathToModels + "white/knight/knight.obj",
            glm::vec3(0, 0.12f, 3.2f),
            { glm::vec2(1, 7), glm::vec2(6, 7) },
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        queenWhite = Figure(
            pathToModels + "white/queen/queen.obj",
            glm::vec3(0, 0.12f, 4.52f),
            { glm::vec2(3, 7) },
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
        rookWhite = Figure(
            pathToModels + "white/rook/rook.obj",
            glm::vec3(0, 0.12f, 2.55f),
            { glm::vec2(0, 7), glm::vec2(7, 7) },
            PIECE_SCALE,
            glm::vec3(0, 180, 0)
        );
	}
private:

};

glm::vec3 GetSquareCoord(glm::vec2 coord) {
    return glm::vec3(coord.x * SQUARE_SIZE, 0, -coord.y * SQUARE_SIZE);
}

#endif