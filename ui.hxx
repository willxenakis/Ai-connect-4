#pragma once

#include "model.hxx"
#include <ge211.hxx>
#include <sstream>

// Sizes of grid and tokens:
int const grid_size                 = 100,
        token_radius              = grid_size / 2;

// Colors of rendered tokens.
ge211::Color const playing_bg   = ge211::Color::medium_yellow().lighten(0.7),
        stalemate_bg = ge211::Color::black(),
        red_color  = ge211::Color::medium_red(),
        blue_color = ge211::Color::medium_blue(),
        green_color = ge211::Color::medium_green(),
        purple_color = ge211::Color::medium_magenta(),
        orange_color = ge211::Color(255, 99,71),
        teal_color = ge211::Color(0,128,128);

// Code for how we interact with the model.
struct Connect4_ui : ge211::Abstract_game
{
    ///
    /// MEMBER FUNCTIONS
    ///
    /// Each of these member functions *overrides* a default implementation
    /// inherited from `ge211::Abstract_game`. For example, the default
    /// implementation of `on_mouse_move` does nothing, but we change it
    /// to keep track of the mouse position.
    ///

    // Displays the current state of the model by adding sprites to the given
    // `Sprite_set`.
    void draw(ge211::Sprite_set&) override;

    // Called by the game engine when the mouse moves.
    void on_mouse_move(ge211::Position) override;

    // Called by the game engine when the mouse button is clicked.
    void on_mouse_down(ge211::Mouse_button, ge211::Position) override;

    // Called by the game engine when a key is clicked.
    void on_key(ge211::Key) override;

    // Returns the dimensions that the window should have (based on the grid
    // dimensions).
    ge211::Dimensions initial_window_dimensions() const override;

    // Helper function for computing the physical dimensions of the board.
    // `static` means it doesn't require a `Connect4_ui` object to call it,
    // which means we can call it when constructing a `Connect4_ui` (and
    // we will).
    static ge211::Dimensions board_pixels();

    // Returns the title to put on the window.
    std::string initial_window_title() const override;

    ///
    /// FIELDS (PRIVATE DATA MEMBERS)
    ///

    // Holds the logical (presentation-independent) state of the game.
    Connect4_model model_;

    // Logical board column where the mouse was last seen.
    int mouse_column_ = -1;

    // The sprites, for displaying player tokens.
    ge211::Circle_sprite player1_token_{token_radius};
    ge211::Circle_sprite player2_token_{token_radius};
    ge211::Circle_sprite player1_shadow_{token_radius, red_color.lighten(0.5)};
    ge211::Circle_sprite player2_shadow_{token_radius, blue_color.lighten(0.5)};
    ge211::Color set_tint_1;
    ge211::Color set_tint_2;

    // Sprite for telling the user how to replay or quit the game
    ge211::Text_sprite const replay{"Press r to replay or q to quit",
                                    ge211::Font("sans.ttf", 20)};

    // Sprite for telling user how to choose whether to play
    // 1st or 2nd at the start of the game
    ge211::Text_sprite const game_start{"Press 1 to play 1st, 2 to play 2nd.",
                                        ge211::Font("sans.ttf", 20)};

    // Sprites for telling user how to choose a color scheme at start of first game
    ge211::Text_sprite const choose_color_scheme_1{"Press \"o\" for orange and teal color scheme",
                                                   ge211::Font("sans.ttf", 20)};
    ge211::Text_sprite const choose_color_scheme_2{"\"r\" for red and blue",
                                                   ge211::Font("sans.ttf", 20)};
    ge211::Text_sprite const choose_color_scheme_3{"or \"g\" for green and purple",
                                                   ge211::Font("sans.ttf", 20)};

    //Black Rectangular Border To Separate Game and Score Counts
    ge211::Rectangle_sprite border_{{2 * token_radius * Connect4_model::m,
                                     30}, ge211::Color::black()};

    ge211::Rectangle_sprite play_best_move{{150,35},
                                           ge211::Color::medium_green()};
    ge211::Text_sprite play_best_move_text{"Play Best Move",
                                           ge211::Font("sans.ttf", 20)};

    ge211::Rectangle_sprite see_best_move{{225,35},
                                          ge211::Color::medium_green()};
    ge211::Text_sprite see_best_move_text{"See Best Previous Move",
                                          ge211::Font("sans.ttf", 20)};

    std::string best_move_string = "Best Move Col: "+std::to_string(3);
    ge211::Text_sprite best_prev_move_text{best_move_string,
                                           ge211::Font("sans.ttf", 15)};
    ge211::Rectangle_sprite best_prev_move_test_box{{125,25},
                                                    ge211::Color::medium_green()};


    //Construct strings that will be used by Text Sprites for Score Counts
    std::string ai_wins_string = "AI Wins: " + std::to_string(0);
    std::string human_wins_string = "Human Wins: " + std::to_string(0);
    std::string ties_string = "Ties: " + std::to_string(0);

    //Score Count Text Sprites to be Displayed Below the Game
    ge211::Text_sprite ai_wins_sprite{ai_wins_string,
                                      ge211::Font("sans.ttf", 50)};
    ge211::Text_sprite human_wins_sprite{human_wins_string,
                                         ge211::Font("sans.ttf", 50)};
    ge211::Text_sprite ties_sprite{ties_string,
                                   ge211::Font("sans.ttf", 50)};

};