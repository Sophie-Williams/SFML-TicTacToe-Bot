// TicTacToe.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <iostream>
#include <map>

constexpr auto BOT_ID = 0;
constexpr auto PLAYER_ID = 1;


sf::VertexArray construct_cross_at(int i) {
	sf::VertexArray cross(sf::Quads, 8);
	double offset_x = 100 * (i / 3);
	double offset_y = 100 * (i % 3);
	cross[0].position = sf::Vector2f(offset_x + 10.f, offset_y + 4.f);
	cross[1].position = sf::Vector2f(offset_x + 4.f, offset_y + 10.f);
	cross[2].position = sf::Vector2f(offset_x + 100.f - 10.f, offset_y + 100 - 4.f);
	cross[3].position = sf::Vector2f(offset_x + 100.f - 4.f, offset_y + 100 - 10.f);
	cross[4].position = sf::Vector2f(offset_x + 100.f - 10.f, offset_y + 4.f);
	cross[5].position = sf::Vector2f(offset_x + 100.f - 4.f, offset_y + 10.f);
	cross[6].position = sf::Vector2f(offset_x + 10.f, offset_y + 100 - 4.f);
	cross[7].position = sf::Vector2f(offset_x + 4.f, offset_y + 100 - 10.f);
	return cross;
}

sf::CircleShape construct_circle_at(int i) {
	double offset_x = 100 * (i / 3);
	double offset_y = 100 * (i % 3);
	sf::CircleShape circle(35.f);
	circle.setFillColor(sf::Color::Red);
	circle.setOutlineColor(sf::Color::Green);
	circle.setOutlineThickness(10.f);
	circle.setPosition(sf::Vector2f(offset_x + 15.f, offset_y + 15.f));
	return circle;
}


class GameLogic {
private:
	std::pair<int, bool> game_state(int board[3][3]) {
		int winner = -1;
		for (int i = 0; i < 3; ++i) {
			if (board[i][0] != -1 && board[i][0] == board[i][1] && board[i][1] == board[i][2]) winner = board[i][0];
			if (board[0][i] != -1 && board[0][i] == board[1][i] && board[1][i] == board[2][i]) winner = board[0][i];
		}
		if (board[1][1] != -1) {
			if (board[1][1] == board[0][0] && board[1][1] == board[2][2]) winner = board[1][1];
			if (board[1][1] == board[2][0] && board[1][1] == board[0][2]) winner = board[1][1];
		}
		bool is_finished = winner != -1;
		if (!is_finished) {
			for (int i = 0; i < 9; ++i) {
				if (board[i / 3][i % 3] == -1) is_finished = false;
			}
		}
		if (winner == PLAYER_ID) return std::make_pair(-1, is_finished);
		else if (winner == BOT_ID) return std::make_pair(1, is_finished);
		return std::make_pair(0, is_finished);
	}
	std::pair<int, int> minimizer_move(int board[3][3]) {
		std::pair<int, bool> gs = game_state(board);
		if (gs.second) return std::make_pair(gs.first, -1);
		std::vector<std::pair<int, int>> possible_moves;
		for (int i = 0; i < 9; ++i) {
			if (board[i / 3][i % 3] != -1) continue;
			int b[3][3];
			for (int j = 0; j < 9; ++j) {
				if (i != j) b[j / 3][j % 3] = board[j / 3][j % 3];
				else b[j / 3][j % 3] = PLAYER_ID;
			}
			std::pair<int, int> move = maximizer_move(b);
			possible_moves.push_back(std::make_pair(move.first, i));
		}
		if (possible_moves.size() == 0) return std::make_pair(0, -1);
		std::sort(possible_moves.begin(), possible_moves.end(), [](std::pair<int, int> p1, std::pair<int, int> p2) {
			return p1.first < p2.first;
		});
		return possible_moves[0];
	}
	std::pair<int, int> maximizer_move(int board[3][3]) {
		std::pair<int, bool> gs = game_state(board);
		if (gs.second) return std::make_pair(gs.first, -1);
		std::vector<std::pair<int, int>> possible_moves;
		for (int i = 0; i < 9; ++i) {
			if (board[i / 3][i % 3] != -1) continue;
			int b[3][3];
			for (int j = 0; j < 9; ++j) {
				if (i != j) b[j / 3][j % 3] = board[j / 3][j % 3];
				else b[j / 3][j % 3] = BOT_ID;
			}
			std::pair<int, int> move = minimizer_move(b);
			possible_moves.push_back(std::make_pair(move.first, i));
		}
		
		if (possible_moves.size() == 0) return std::make_pair(0, -1);
		std::sort(possible_moves.begin(), possible_moves.end(), [](std::pair<int, int> p1, std::pair<int, int> p2) {
			return p1.first > p2.first;
		});
		return possible_moves[0];
	}
public:
	int board[3][3];
	int winner;
	std::vector<sf::CircleShape> circles;
	std::vector<sf::VertexArray> verts;
public:
	GameLogic() {
		reset_game();
	}

	void reset_game() {
		winner = -1;
		circles.clear();
		verts.clear();
		for (int i = 0; i < 9; ++i) board[i / 3][i % 3] = -1;
	}

	bool finished_game() {
		for (int i = 0; i < 3; ++i) {
			if (board[i][0] != -1 && board[i][0] == board[i][1] && board[i][1] == board[i][2]) winner = board[i][0];
			if (board[0][i] != -1 && board[0][i] == board[1][i] && board[1][i] == board[2][i]) winner = board[0][i];
		}
		if (board[1][1] != -1) {
			if (board[1][1] == board[0][0] && board[1][1] == board[2][2]) winner = board[1][1];
			if (board[1][1] == board[2][0] && board[1][1] == board[0][2]) winner = board[1][1];
		}
		bool is_finished = true;
		for (int i = 0; i < 9; ++i) {
			if (board[i / 3][i % 3] == -1) is_finished = false;
		}
		if (is_finished == true) winner = -2;
		return winner != -1 || is_finished;
	}

	bool apply_player_move(int i) {
		board[i / 3][i % 3] = 1;
		circles.push_back(construct_circle_at(i));
		return finished_game();
	}

	bool apply_bot_move() {
		std::pair<int, int> move = maximizer_move(this->board);
		int tile = move.second;
		board[tile / 3][tile % 3] = 0;
		verts.push_back(construct_cross_at(tile));
		return finished_game();
	}

	bool perform_game_turn(int i) {
		if (apply_player_move(i)) return true;
		print_board();
		if (apply_bot_move()) return true;
		print_board();
		return false;
	}

	void print_board() {
		for (int j = 0; j < 3; ++j) {
			for (int i = 0; i < 3; ++i) {
				if (board[i][j] == 1) std::cerr << "O";
				else if (board[i][j] == 0) std::cout << "X";
				else std::cout << "-";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};

int position_of_tile(int x, int y) {
	return x / 100 * 3 + y / 100;
}


int main()
{
	sf::Font font;
	sf::FloatRect bounds;

	if (!font.loadFromFile("arial.ttf"))
	{
		std::cerr << "font loading error" << std::endl;
	}
	sf::RenderWindow window(sf::VideoMode(450, 300), "SFML TicTacToe", sf::Style::Titlebar | sf::Style::Close);

	sf::Text text1;
	text1.setFont(font);
	text1.setString("Winner");
	text1.setCharacterSize(30);
	text1.setColor(sf::Color::Red);

	sf::Text win_lose_text;
	win_lose_text.setFont(font);
	win_lose_text.setCharacterSize(30);
	win_lose_text.setColor(sf::Color::Red);


	std::vector<sf::RectangleShape> rects;
	for (int i = 0; i < 9; ++i) {
		double x = 100 * (i / 3), y = 100 * (i % 3);
		sf::RectangleShape r(sf::Vector2f(96.f, 96.f));
		r.setFillColor(sf::Color::Red);
		r.setPosition(sf::Vector2f(x + 2.f, y + 2.f));
		rects.push_back(r);
	}

	sf::RectangleShape replay_button(sf::Vector2f(100, 50));
	replay_button.setFillColor(sf::Color::Red);
	replay_button.setOutlineColor(sf::Color::Blue);
	bounds = replay_button.getLocalBounds();
	replay_button.setPosition(sf::Vector2f(300.f + (150.f - bounds.width) / 2.f, 200.f));

	sf::Text replay_text;
	replay_text.setFont(font);
	replay_text.setCharacterSize(25);
	replay_text.setString("REPLAY");
	bounds = replay_text.getLocalBounds();
	replay_text.setPosition(sf::Vector2f(300.f + (150.f - bounds.width) / 2.f, 190.f + (50.f - bounds.height) / 2.f));


	GameLogic game;

	while (window.isOpen())
	{
		bool mouse_pressed = false;
		bool restart_hovered = false;
		bool restart_pressed = false;
		int mouse_x, mouse_y;
		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed) {
				mouse_pressed = true;
			}

			if (event.type == sf::Event::MouseMoved) {
				sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
				mouse_x = mouse_pos.x;
				mouse_y = mouse_pos.y;
			}
		}


		if (mouse_pressed && game.winner == -1) {
			int tile = position_of_tile(mouse_x, mouse_y);
			if (tile >= 0 && tile < 9) {
				game.perform_game_turn(tile);
			}
			std::cout << tile << std::endl;
		}
		if (mouse_pressed && mouse_x > (300.f + (150.f - bounds.width) / 2.f) && mouse_x < (300.f + (150.f + bounds.width) / 2.f) && mouse_y > 200 && mouse_y < 250) {
			game.reset_game();
		}

		if (mouse_x > (300.f + (150.f - bounds.width) / 2.f) && mouse_x < (300.f + (150.f + bounds.width) / 2.f) && mouse_y > 200 && mouse_y < 250) {
			replay_button.setOutlineThickness(2);
			replay_text.setColor(sf::Color::Cyan);
		}
		else {
			replay_button.setOutlineThickness(0);
			replay_text.setColor(sf::Color::Green);
		}

		window.clear(sf::Color::Green);
		window.draw(replay_button);
		window.draw(replay_text);
		for (sf::RectangleShape r : rects) window.draw(r);
		for (sf::CircleShape c : game.circles) window.draw(c);
		for (sf::VertexArray v : game.verts) window.draw(v);
		bounds = text1.getLocalBounds();
		text1.setPosition(sf::Vector2f(300.f + (150.f - bounds.width) / 2.f, 0.f));
		window.draw(text1);
		if (game.winner == BOT_ID) win_lose_text.setString("BOT");
		else if (game.winner == PLAYER_ID) win_lose_text.setString("YOU");
		else if (game.winner == -2) win_lose_text.setString("TIE");
		else win_lose_text.setString("PLAYING");
		bounds = win_lose_text.getLocalBounds();
		win_lose_text.setPosition(sf::Vector2f(300.f + (150.f - bounds.width) / 2.f, 40.f));
		window.draw(win_lose_text);
		window.display();
	}
	std::cout << game.winner << std::endl;
	return 0;
}
