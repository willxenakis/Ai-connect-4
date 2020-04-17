#include "model.hxx"

// For `std::logic_error` and `std::invalid_argument`:
#include <stdexcept>
#include <sstream>


const int MAX_DEPTH = 5;

Player other_player(Player p)
{
    switch (p) {
        case Player::human:
            return Player::ai;
        case Player::ai:
            return Player::human;
        default:
            throw std::invalid_argument("other_player: not a player");
    }
}

// Constructor for Connect4_model.
//
// The second line (`: board_(m)`) constructs the `board_` member
// variable. Since `board_` is declared like
//
//     std::vector<column_t> board_;
//
// the member initializer is as if we were constructing it like
//
//     std::vector<column_t> board_(m);
//
// which initializes `board_` to have `m` elements, each of which
// is a default-constructed (empty) `column_t`.
Connect4_model::Connect4_model()
        : board_(m)
{ }


const int Connect4_model::k = 4;  // how many to connect
const int Connect4_model::m = 7;  // grid width
const int Connect4_model::n = 6;  // grid height

void Connect4_model::place_token(int col_no)
{
    check_playable_(col_no);
    board_[col_no].push_back(turn_);

    update_choice_(col_no);

    update_winner_and_turn_();
}

const Connect4_model::column_t& Connect4_model::column(int col_no) const
{
    check_column_(col_no);
    return board_[col_no];
}

bool Connect4_model::is_good_column(int col_no) const
{
    return 0 <= col_no && col_no < m;
}

bool Connect4_model::is_playable(int col_no) const
{
    return turn() != Player::neither &&
           is_good_column(col_no) &&
           column(col_no).size() < Connect4_model::n;
}

void Connect4_model::update_winner_and_turn_()
{
    Posn move{col_choice_, (int)board_[col_choice_].size() - 1};

    int below       = count_from_by_(move, 0, -1, turn_);
    int left        = count_from_by_(move, -1, 0, turn_);
    int right       = count_from_by_(move, 1, 0, turn_);
    int above_left  = count_from_by_(move, -1, 1, turn_);
    int above_right = count_from_by_(move, 1, 1, turn_);
    int below_left  = count_from_by_(move, -1, -1, turn_);
    int below_right = count_from_by_(move, 1, -1, turn_);

    if (below + 1 >= k ||
        left + 1 + right >= k ||
        above_left + 1 + below_right >= k ||
        above_right + 1 + below_left >= k) {
        winner_ = turn_;
        if(turn_==Player::human) human_wins++;
        if(turn_==Player::ai) ai_wins++;
    } else {
        for (column_t const& column : board_) {
            if (column.size() < n) {
                turn_ = other_player(turn_);
                return;
            }
        }
    }

    turn_ = Player::neither;

    //Needs to be a check in model for a tie for this to work
}

void Connect4_model::update_choice_(int col_no) {
    col_choice_ = col_no;
}

void Connect4_model::ai_move() {
    mmMove_ choice = max_(0, board_);

    place_token(choice.index);
}

void Connect4_model::human_ai_move(){
    mmMove_ choice = max_(0, board_);

    place_token(choice.index);
}

mmMove_ Connect4_model::max_(int depth,
                             std::vector<column_t> curr_board) {
    std::vector<int> good_cols;

    for (int i=0; i<m; i++) {
        if (curr_board[i].size() < n)
            good_cols.push_back(i);
    }

    int score = score_board_(curr_board);

    if (depth >= MAX_DEPTH ||
        score == 999999 ||
        score == -999999 ||
        good_cols.empty()) {
        return {-1, score};
    }

    mmMove_ best = {-1, -999999};

    for (int i=0; i<good_cols.size(); i++) {
        std::vector<column_t> new_board(curr_board);

        new_board[good_cols[i]].push_back(Player::ai);


        mmMove_ move = {good_cols[i],
                        mini_(
                                depth+1,
                                new_board).score};

        if (best.index == -1 || best.score < move.score)
            best = move;
    }

    return best;
}

mmMove_ Connect4_model::mini_(int depth,
                              std::vector<column_t> curr_board) {
    std::vector<int> good_cols;

    for (int i=0; i<m; i++) {
        if (curr_board[i].size() < n)
            good_cols.push_back(i);
    }

    int score = score_board_(curr_board);

    if (depth >= MAX_DEPTH ||
        score == 999999 ||
        score == -999999 ||
        good_cols.empty()) {
        return {-1, score};
    }

    mmMove_ best = {-1, 999999};

    for (int i=0; i<good_cols.size(); i++) {
        std::vector<column_t> new_board(curr_board);

        new_board[good_cols[i]].push_back(Player::human);


        mmMove_ move = {good_cols[i],
                        max_(
                                depth+1,
                                new_board).score};

        if (best.index == -1 || best.score > move.score)
            best = move;
    }

    return best;
}

int Connect4_model::score_board_(std::vector<column_t> const& curr_board) {
    int hScore = 0;
    int aiScore = 0;
    int score = 0;

    // Get column pts
    for (int i=0; i<n-3; i++) {
        for (int j=0; j<m; j++) {
            hScore = sum_from_by_(curr_board, {j,i}, 0, 1, Player::human);
            aiScore = sum_from_by_(curr_board, {j,i}, 0, 1, Player::ai);

            if (hScore == 999999) return -999999;
            if (aiScore == 999999) return 999999;

            score += aiScore;
        }
    }

    // Get row pts
    for (int i=0; i<n; i++) {
        for (int j=0; j<m-3; j++) {
            hScore = sum_from_by_(curr_board, {j,i}, 1, 0, Player::human);
            aiScore = sum_from_by_(curr_board, {j,i}, 1, 0, Player::ai);

            if (hScore == 999999) return -999999;
            if (aiScore == 999999) return 999999;

            score += aiScore;
        }
    }

    // Get diagonal (^, >) pts
    for (int i=0; i<n-3; i++) {
        for (int j=0; j<m-3; j++) {
            hScore = sum_from_by_(curr_board, {j,i}, 1, 1, Player::human);
            aiScore = sum_from_by_(curr_board, {j,i}, 1, 1, Player::ai);

            if (hScore == 999999) return -999999;
            if (aiScore == 999999) return 999999;

            score += aiScore;
        }
    }

    // Get diagonal (^, <) pts
    for (int i=3; i<n; i++) {
        for (int j=0; j<=m-4; j++) {
            hScore = sum_from_by_(curr_board, {j,i}, 1, -1, Player::human);
            aiScore = sum_from_by_(curr_board, {j,i}, 1, -1, Player::ai);

            if (hScore == 999999) return -999999;
            if (aiScore == 999999) return 999999;

            score += aiScore;
        }
    }

    return score;
}

void Connect4_model::check_column_(int col_no) const
{
    if (!is_good_column(col_no)) {
        std::string throw_string = "Model::place_token: column out of bounds";
        throw std::invalid_argument(throw_string);
    }
}

void Connect4_model::check_playable_(int col_no) const
{
    check_column_(col_no);

    if (is_game_over())
        throw std::logic_error("Model::place_token: game over");

    if (!is_playable(col_no))
        throw std::invalid_argument("Model::place_token: column full");
}

int Connect4_model::count_from_by_(Posn start, int dcol,
        int drow, Player curr_turn) const
{
    int count = 0;

    for (;;) {
        start.row += drow;
        start.col += dcol;

        if (!is_occupied_(start.col, start.row))
            return count;

        if (board_[start.col][start.row] != curr_turn)
            return count;

        ++count;
    }
}

int Connect4_model::sum_from_by_(std::vector<column_t> const& curr_board,
        Posn start, int dcol, int drow, Player curr_turn)
{
    int count = 0;
    bool occupied;

    for (int i=0; i<k; i++) {
        occupied = 0 <= start.col && start.col < m &&
                   0 <= start.row &&
                   start.row < curr_board[start.col].size();

        if (occupied && curr_board[start.col][start.row] == curr_turn)
            count++;

        start.col += dcol;
        start.row += drow;
    }

    if (count >= k)
        return 999999;

    return count;
}

bool Connect4_model::is_occupied_(int col_no, int row_no) const
{
    return is_good_column(col_no) &&
           0 <= row_no && row_no < board_[col_no].size();
}

void Connect4_model::theoretical_best_human_move(){

    std::vector<column_t> new_board(board_);

    new_board[best_prev_move].pop_back();

    mmMove_ best_move = mini_(0, new_board);

    best_prev_move = best_move.index;
}