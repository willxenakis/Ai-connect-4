#pragma once

#include <vector>

// How we represent players or the absence thereof.
enum class Player
{
    human,
    ai,
    neither,
};

// How minimax keeps track of recursive scores
struct mmMove_ {
    int index;
    int score;
};

// Returns the other player, if given Player::first or Player::second;
// throws std::invalid_argument if given Player::neither.
Player other_player(Player);

// Models a Connect Four game.
struct Connect4_model
{
    ///
    /// A TYPE ABBREVIATION:
    ///

    // A type member used below - we represent a column as a vector of
    // `Player`s:
    using column_t = std::vector<Player>;


    ///
    /// CONSTRUCTOR
    ///

    // Constructs an empty Connect Four game model.
    Connect4_model();


    ///
    /// CONSTANTS
    ///

    // Game size parameters.
    // (Defined in model.cxx.)
    static const int k;  // how many to connect (4)
    static const int m;  // grid width (7)
    static const int n;  // grid height (6)


    ///
    /// API FUNCTIONS (PUBLIC MEMBER FUNCTIONS)
    ///

    // Places the token for the current player in the given column.
    //
    // **PRECONDITION**: `is_playable(col_no)` (throws)
    void place_token(int col_no);


    // Returns whose turn it is, or Player::neither for game over.
    Player turn() const { return turn_; };

    // Returns the winner if there is one; returns Player::neither for
    // stalemates or when the game isn't over yet.
    Player winner() const { return winner_; };

    // Gets a read-only view of the given column.
    //
    // **PRECONDITION:** `is_good_col(col_no)` (throws)
    const column_t& column(int col_no) const;


    // Is the column number within bounds?
    bool is_good_column(int col_no) const;

    // Can we play in the given column? Returns true if the game is
    // not over and the column is not full.
    bool is_playable(int col_no) const;

    // Is the game over?
    bool is_game_over() const { return turn_ == Player::neither; }

    ///
    /// INTERNAL HELPER FUNCTIONS ("PRIVATE" MEMBER FUNCTIONS)
    ///

    // Checks for a winner, or advances the turn if there isn't one.
    //
    // **PRECONDITION** `col_no` where the most recent move was played
    // (whch also means it's in bounds).
    void update_winner_and_turn_();

    // A cheap position type (to avoid model depending on GE211).
    struct Posn { int col, row; };

    void update_choice_(int col_no);

    int score_board_(std::vector<column_t> const& curr_board);

    mmMove_ mini_(int depth,
                  std::vector<column_t> curr_board);
    mmMove_ max_(int depth,
                 std::vector<column_t> curr_board);

    void ai_move();

    void human_ai_move();

    void theoretical_best_human_move();

    // Counts the number of instances of `turn_`, moving in direction
    // {dcol, drow} starting at `start`, and *not* counting the Player
    // at the starting position.
    int count_from_by_(Posn start, int dcol, int drow, Player curr_turn) const;

    int sum_from_by_(std::vector<column_t> const& curr_board, Posn start,
            int dcol, int drow, Player curr_turn);

    // Returns whether there is a token at the given position.
    bool is_occupied_(int col_no, int row_no) const;

    // Checks that `col_no` is in bounds, throwing an exception if not.
    void check_column_(int col_no) const;

    // Checks that we can play in `col_no`, throwing an exception
    // if we can't.
    void check_playable_(int col_no) const;

    ///
    /// FIELDS (PRIVATE DATA MEMBERS)
    ///

    // The grid of tokens, by column. Each column vector is indexed from
    // the bottom and only as long as the number of player tokens in it.
    // (It ISN'T padded to the full board height with `Player::neither`s,
    // so `board_` can be thought of as a "ragged" 2-D array.)
    std::vector<column_t> board_;

    //Has the game started?
    bool game_started_ = false;

    bool color_scheme_chosen_ = false;

    bool game_over_for_ties = false;

    int human_wins = 0;

    int ai_wins = 0;

    int ties = 0;

    bool mouse_below_border = false;

    bool human_has_played_once = false;

    char color_scheme = 'r';

    // The current turn.
    Player turn_   = Player::human;

    // The winning player, if any.
    Player winner_ = Player::neither;

    int col_choice_ = 0;

    int best_prev_move = 0;

    // INVARIANT (game invalid if false):
    //
    //  - board_.size() == Model::m
    //
    //  - for (column_t c : board_) c.size() <= Model::n
    //
    //  - for (column_t c : board_) for (Player p : c) p != Player::neither
    //
    //  - turn_ == Player::neither || winner_ == Player::neither
    //
    //  - If `turn_ != Player::neither` then there is no line of length
    //    `Model::k` on the board.
    //
    //  - For either `Player p`, `winner_ == p` if and only if there is a
    //    length-`Model::k` line of `p` tokens on the board.
};