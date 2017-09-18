function myFunction(){
jQuery('.div').click(function() {
    $(this).nextUntil(':not(.toggle)').slideToggle();
});
}
window.onload=myFunction;