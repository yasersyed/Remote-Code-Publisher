$(".header").click(function () {
    $header = $(this);
    $content = $header.next();
    $content.slideToggle(500, function () {
        $header.text(function () {
            //change text based on condition
            return $content.is(":visible") ? "{" : "{..}";
        });
    });

});