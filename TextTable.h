#include <iostream>
#include <map>
#include <iomanip>
#include <vector>
#include <string>

class TextTable {

    public:
    enum class Alignment { LEFT, RIGHT }; 
    typedef std::vector< std::string > Row;
    TextTable( char horizontal = '-', char vertical = '|', char corner = '+' );
    void setAlignment( unsigned i, Alignment alignment );
    Alignment alignment( unsigned i ) const;
    char vertical() const;
    char horizontal() const;
    void add( std::string const & content );
    void endOfRow();
    template <typename Iterator>
    void addRow( Iterator begin, Iterator end );
    template <typename Container>
    void addRow( Container const & container );
    std::vector< Row > const & rows() const;
    void setup() const;
    std::string ruler() const;
    int width( unsigned i ) const;
    friend std::ostream & operator << (std::ostream &out, TextTable const & table);

    private:
    char _horizontal;
    char _vertical;
    char _corner;
    Row _current;
    std::vector< Row > _rows;
    std::vector< unsigned > mutable _width;
    std::map< unsigned, Alignment > mutable _alignment;
    static std::string repeat( unsigned times, char c );
    unsigned columns() const;
    void determineWidths() const;
    void setupAlignment() const;
};

