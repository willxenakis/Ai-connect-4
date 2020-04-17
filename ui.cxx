#include "ui.hxx"

using namespace ge211;

// Converts a logical board position to the physical screen position
// of the upper-left corner of the corresponding grid square.
static Position board_to_screen_(Position board_pos)
{
    int x = 2 * token_radius * board_pos.x;
    int y = 2 * token_radius * (Connect4_model::n - board_pos.y - 1);
    return {x, y};
}

// Converts a physical screen position to the logical board position
// that corresponds to it.
static Position screen_to_board_(Position screen_pos)
{
    int col_no = screen_pos.x / (2 * token_radius);
    int row_no = Connect4_model::n - screen_pos.y / (2 * token_radius) - 1;
    return {col_no, row_no};
}

void Connect4_ui::draw(ge211::Sprite_set& sprites) {
    sprites.add_sprite(border_, {0,
                                 2 * token_radius * Connect4_model::n+5},5);

    if(model_.color_scheme=='r'){
        player1_token_.recolor(red_color);
        player1_shadow_.recolor(red_color.lighten(0.5));
        player2_token_.recolor(blue_color);
        player2_shadow_.recolor(blue_color.lighten(0.5));
        set_tint_1=red_color.lighten(0.5);
        set_tint_2=blue_color.lighten(0.5);
    }
    else if(model_.color_scheme=='o'){
        player1_token_.recolor(orange_color);
        player1_shadow_.recolor(orange_color.lighten(0.5));
        player2_token_.recolor(teal_color);
        player2_shadow_.recolor(teal_color.lighten(0.5));
        set_tint_1=orange_color.lighten(0.5);
        set_tint_2=teal_color.lighten(0.5);
    }
    else{
        player1_token_.recolor(green_color);
        player1_shadow_.recolor(green_color.lighten(0.5));
        player2_token_.recolor(purple_color);
        player2_shadow_.recolor(purple_color.lighten(0.5));
        set_tint_1=green_color.lighten(0.5);
        set_tint_2=purple_color.lighten(0.5);
    }

    if(!model_.game_started_){
        sprites.add_sprite(game_start, {board_pixels().width/4,
                                        board_pixels().height/2});
    }
    else if(!model_.color_scheme_chosen_){
        //Add text sprites to choose color scheme
        sprites.add_sprite(choose_color_scheme_1,{board_pixels().width/4,
                                                  board_pixels().height/2});
        sprites.add_sprite(choose_color_scheme_2,{board_pixels().width/4,
                                                  board_pixels().height/2+20});
        sprites.add_sprite(choose_color_scheme_3,{board_pixels().width/4,
                                                  board_pixels().height/2+40});
    }
    else {

        //Add buton sprites
        sprites.add_sprite(see_best_move, {2 * token_radius *
        Connect4_model::m - 225,2 * token_radius *
        Connect4_model::n + 35}, 5);

        sprites.add_sprite(play_best_move, {0, 2 * token_radius *
        Connect4_model::n + 35}, 5);

        sprites.add_sprite(see_best_move_text, {2 * token_radius
        * Connect4_model::m - 220,2 * token_radius
        * Connect4_model::n + 30}, 6);

        sprites.add_sprite(play_best_move_text, {3, 2 * token_radius *
        Connect4_model::n + 30}, 6);

        sprites.add_sprite(best_prev_move_text, {2 * token_radius
        * Connect4_model::m - 165,2 * token_radius
        * Connect4_model::n + 70}, 6);

        sprites.add_sprite(best_prev_move_test_box, {2 * token_radius
        * Connect4_model::m - 170,2 * token_radius
        * Connect4_model::n + 70}, 5);

        //Add scores sprites
        sprites.add_sprite(ai_wins_sprite, {200, 2 * token_radius
        * Connect4_model::n+50},6);

        sprites.add_sprite(human_wins_sprite, {200, 2 * token_radius
        * Connect4_model::n+125},6);

        sprites.add_sprite(ties_sprite, {200, 2 * token_radius
        * Connect4_model::n+200}, 6);


        // Here we add a sprite for each token in the game:
        for (int col_no = 0; col_no < Connect4_model::m; ++col_no) {
            const Connect4_model::column_t &column = model_.column(col_no);
            for (int row_no = 0; row_no < column.size(); ++row_no) {
                Player player = column[row_no];
                Position screen_pos = board_to_screen_({col_no, row_no});

                if (player == Player::human)
                    sprites.add_sprite(player1_token_, screen_pos);
                else
                    sprites.add_sprite(player2_token_, screen_pos);
            }
        }

        // Here we possibly add a sprite as a cursor that follows the mouse:
        if (model_.is_playable(mouse_column_) && !model_.mouse_below_border) {
            int col_no = mouse_column_,
                    row_no = (int) model_.column(col_no).size();
            Position screen_pos = board_to_screen_({col_no, row_no});

            if (model_.turn() == Player::human)
                sprites.add_sprite(player1_shadow_, screen_pos);
            else
                sprites.add_sprite(player2_shadow_, screen_pos);
        }

        // Select the background color for the window based on the
        // winner or lack thereof. Also update scores sprites
        if (model_.winner() == Player::human) {
            background_color = set_tint_1;
            sprites.add_sprite(replay, {board_pixels().width/3,
                                        2 * token_radius
                                        * Connect4_model::n}, 6);

            ai_wins_string = "AI Wins: " + std::to_string(model_.ai_wins);
            human_wins_string = "Human Wins: " +
                    std::to_string(model_.human_wins);

            ai_wins_sprite = {ai_wins_string,
                              ge211::Font("sans.ttf", 50)};
            human_wins_sprite = {human_wins_string,
                                 ge211::Font("sans.ttf", 50)};

        } else if (model_.winner() == Player::ai) {
            background_color = set_tint_2;
            sprites.add_sprite(replay, {board_pixels().width/3,
                                        2 * token_radius
                                        * Connect4_model::n}, 6);

            ai_wins_string = "AI Wins: " +
                    std::to_string(model_.ai_wins);
            human_wins_string = "Human Wins: " +
                    std::to_string(model_.human_wins);

            ai_wins_sprite = {ai_wins_string,
                              ge211::Font("sans.ttf", 50)};
            human_wins_sprite = {human_wins_string,
                                 ge211::Font("sans.ttf", 50)};
        } else if (model_.is_game_over()) {
            if(!model_.game_over_for_ties){
                model_.ties+=1;
                model_.game_over_for_ties = true;
            }
            background_color = stalemate_bg.lighten(0.75);
            sprites.add_sprite(replay, {board_pixels().width/3,
                                        2 * token_radius
                                        * Connect4_model::n}, 6);

            ai_wins_string = "AI Wins: " + std::to_string(model_.ai_wins);
            human_wins_string = "Human Wins: " +
                    std::to_string(model_.human_wins);
            ties_string = "Ties: " + std::to_string(model_.ties);

            ai_wins_sprite = {ai_wins_string,
                              ge211::Font("sans.ttf", 50)};
            human_wins_sprite = {human_wins_string,
                                 ge211::Font("sans.ttf", 50)};
            ties_sprite = {ties_string, ge211::Font("sans.ttf", 50)};
        } else
            background_color = playing_bg;
    }
}

void Connect4_ui::on_mouse_move(Position screen_pos)
{
    if(screen_pos.y< 2 * token_radius * Connect4_model::n+5){
        mouse_column_ = screen_to_board_(screen_pos).x;
        model_.mouse_below_border = false;
    }
    else{
        model_.mouse_below_border = true;
    }
}

Dimensions Connect4_ui::initial_window_dimensions() const
{
    return board_pixels();
}

ge211::Dimensions Connect4_ui::board_pixels()
{
    return {2 * token_radius * Connect4_model::m,
            2 * token_radius * Connect4_model::n+300};
}

std::string Connect4_ui::initial_window_title() const
{
    return "Connect Four";
}

void Connect4_ui::on_mouse_down(Mouse_button btn, Position screen_posn)
{
    if(model_.game_started_ && model_.color_scheme_chosen_) {
        if(screen_posn.x > 2 * token_radius * Connect4_model::m - 225
            && screen_posn.y > 2 * token_radius * Connect4_model::n + 35
            && screen_posn.y < 2 * token_radius * Connect4_model::n + 70)
        {
            if(model_.human_has_played_once) {
                //See best last move for human
                model_.theoretical_best_human_move();
                best_move_string = "Best Move Col: " +
                        std::to_string(model_.best_prev_move+1);
                best_prev_move_text = {best_move_string,
                                       ge211::Font("sans.ttf", 15)};

            }
        }
        else if(screen_posn.x < 150
                && screen_posn.y > 2 * token_radius * Connect4_model::n + 35
                && screen_posn.y < 2 * token_radius * Connect4_model::n + 70)
        {
            //Play best move
            if (model_.turn() == Player::neither || btn != Mouse_button::left)
                return;

            model_.human_ai_move();
            model_.ai_move();
        }
        else if(screen_posn.y< 2 * token_radius * Connect4_model::n+5){
            if (model_.turn() == Player::neither || btn != Mouse_button::left)
                return;

            int col_no = screen_to_board_(screen_posn).x;

            if (!model_.is_playable(col_no)){
                return;
            }

            model_.place_token(col_no);

            model_.human_has_played_once = true;
            model_.best_prev_move=col_no;

            model_.ai_move();
        }
    }
}

void Connect4_ui::on_key(Key key)
{
    // If q is pressed at any time, quit game
    if(key==Key::code('q')){
        quit();
    }

    // If r is pressed once the game is over, save scores, reset the board
    // and store score in new model
    if(key==Key::code('r')){
        if(model_.is_game_over()){
            int human_wins = model_.human_wins;
            int ai_wins = model_.ai_wins;
            int ties = model_.ties;
            char color_scheme = model_.color_scheme;
            model_= Connect4_model();
            model_.human_wins = human_wins;
            model_.ai_wins = ai_wins;
            model_.ties = ties;
            model_.color_scheme = color_scheme;
            model_.color_scheme_chosen_ = true;
        }
    }

    // If 1 is pressed before the game has started, human plays first
    if(key==Key::code('1')){
        if(!model_.game_started_){
            model_.game_started_=true;
        }
    }

    // If 2 is pressed before the game has started, human plays second
    if(key==Key::code('2')){
        if(!model_.game_started_){
            model_.turn_=Player::ai;
            model_.game_started_=true;
            model_.ai_move();
        }
    }

    // If o is pressed before the first game has started,
    // set color scheme to orange and teal
    if(key==Key::code('o') && !model_.color_scheme_chosen_){
        model_.color_scheme='o';
        model_.color_scheme_chosen_=true;
    }

    // If r is pressed before the first game has started,
    // set color scheme to red and blue
    if(key==Key::code('r') && !model_.color_scheme_chosen_){
        model_.color_scheme='r';
        model_.color_scheme_chosen_=true;
    }

    // If g is pressed before the first game has started,
    // set color scheme to green and purple
    if(key==Key::code('g') && !model_.color_scheme_chosen_){
        model_.color_scheme = 'g';
        model_.color_scheme_chosen_=true;
    }
}