" <O.o>

" Make tabs 2 spaces, and when making a new line, indent to the current line's
" indentation.
set tabstop=2 softtabstop=2 shiftwidth=2 expandtab autoindent

" Add an include guard the include statement on the current line. Assumes the
" cursor starts at column 0.
let @g='f.bveyko#ifndef pa_hbveUveyo#  define pj0a  o#endif€ýa'

" Uppercase/lowercase the current word. Assumes the cursor is not placed on the
" first letter, but may be on the space immediately after the word.
let @u='bveUe'
let @l='bveue'

" Switch to previous/next tab
let @p=':tabp'
let @n=':tabn'

" Open prompt to type name of file to open, creating it if it doesn't exist.
let @t=':tabnew '

" Write an if/else error block pattern with an empty if condition and empty
" error message.
let @e='iif(){}€ýaiko  fprintf();histderr, ""€ýaoreturn();hiexit_failurebveU€ýakkhi€ýal'
