#include "tictactoe.hpp"



void tictactoe::create(const name &challenger, name &host) {
    require_auth(host);
    check(challenger != host, "Challenger should not be the same as the host.");


    games existingHostGames(get_self(), host.value);
    auto itr = existingHostGames.find(challenger.value);
    check(itr == existingHostGames.end(), "Game already exists.");

    existingHostGames.emplace(host, [&](auto &g) {
        g.challenger = challenger;
        g.host = host;
        g.turn = host;
    });
}

void tictactoe::restart(const name &challenger, const name &host, const name &by)
{
    check(has_auth(by), "Only " + by.to_string() + "can restart the game.");


    games existingHostGames(get_self(), host.value);
    auto itr = existingHostGames.find(challenger.value);
    check(itr != existingHostGames.end(), "Game does not exist.");


    check(by == itr->host || by == itr->challenger, "This is not your game.");


    existingHostGames.modify(itr, itr->host, [](auto &g) {
        g.resetGame();
    });
}

void tictactoe::close(const name &challenger, const name &host)
{
    check(has_auth(host), "Only the host can close the game.");

    require_auth(host);


    games existingHostGames(get_self(), host.value);
    auto itr = existingHostGames.find(challenger.value);
    check(itr != existingHostGames.end(), "Game does not exist.");


    existingHostGames.erase(itr);
}

bool tictactoe::isEmptyCell(const uint8_t &cell)
{
    return cell == 0;
}

bool tictactoe::isValidMove(const uint16_t &row, const uint16_t &column, const std::vector<uint8_t> &board)
{
    uint32_t movementLocation = row * game::boardWidth + column;
    bool isValid = movementLocation < board.size() && isEmptyCell(board[movementLocation]);
    return isValid;
}

name tictactoe::getWinner(const game &currentGame)
{
    auto &board = currentGame.board;

    bool isBoardFull = true;


    std::vector<uint32_t> consecutiveColumn(game::boardWidth, 3);
    std::vector<uint32_t> consecutiveRow(game::boardHeight, 3);
    uint32_t consecutiveDiagonalBackslash = 3;
    uint32_t consecutiveDiagonalSlash = 3;

    for (uint32_t i = 0; i < board.size(); i++)
    {
        isBoardFull &= isEmptyCell(board[i]);
        uint16_t row = uint16_t(i / game::boardWidth);
        uint16_t column = uint16_t(i % game::boardWidth);


        consecutiveRow[column] = consecutiveRow[column] & board[i];
        consecutiveColumn[row] = consecutiveColumn[row] & board[i];

        if (row == column)
        {
            consecutiveDiagonalBackslash = consecutiveDiagonalBackslash & board[i];
        }

        if (row + column == game::boardWidth - 1)
        {
            consecutiveDiagonalSlash = consecutiveDiagonalSlash & board[i];
        }
    }


    std::vector<uint32_t> aggregate = {consecutiveDiagonalBackslash, consecutiveDiagonalSlash};
    aggregate.insert(aggregate.end(), consecutiveColumn.begin(), consecutiveColumn.end());
    aggregate.insert(aggregate.end(), consecutiveRow.begin(), consecutiveRow.end());

    for (auto value : aggregate)
    {
        if (value == 1)
        {
            return currentGame.host;
        }
        else if (value == 2)
        {
            return currentGame.challenger;
        }
    }

    return isBoardFull ? draw : none;
}

void tictactoe::move(const name &challenger, const name &host, const name &by, const uint16_t &row, const uint16_t &column)
{
    check(has_auth(by), "The next move should be made by " + by.to_string());


    games existingHostGames(get_self(), host.value);
    auto itr = existingHostGames.find(challenger.value);
    check(itr != existingHostGames.end(), "Game does not exist.");


    check(itr->winner == none, "The game has ended.");

    check(by == itr->host || by == itr->challenger, "This is not your game.");

    check(by == itr->turn, "it's not your turn yet!");


    check(isValidMove(row, column, itr->board), "Not a valid movement.");


    const uint8_t cellValue = itr->turn == itr->host ? 1 : 2;
    const auto turn = itr->turn == itr->host ? itr->challenger : itr->host;
    existingHostGames.modify(itr, itr->host, [&](auto &g) {
        g.board[row * game::boardWidth + column] = cellValue;
        g.turn = turn;
        g.winner = getWinner(g);
    });
}
