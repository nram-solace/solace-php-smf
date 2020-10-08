<!--
    Trying to call PHP from HTML via AJAX
    Bacic plimbing works
    But you can call only call whole PHP script, not functions within the script
    That won't worl for Solace wrapper as the session object needs to be presisted
    between connect and publish
-->
    <html> 
    <head> 
        <title>jQuery PHP</title> 
        <script src="/nram/js/jquery.js"></script>
        <script type='text/javascript'> 
            $(document).ready(function(){ 
            $("#search_results").slideUp(); 
                $("#search_button").click(function(e){ 
                    e.preventDefault(); 
                    ajax_search(); 
                }); 
                $("#search_term").keyup(function(e){ 
                    e.preventDefault(); 
                    ajax_search(); 
                }); 
            
            });
            function ajax_search(){ 
                $("#search_results").show(); 
                var search_val=$("#search_term").val(); 
                $.post("./test2-find.php", {search_term : search_val}, function(data){
                if (data.length>0){ 
                    $("#search_results").html(data); 
                } 
                }) 
            } 
        </script>
    </head> 
 
    <body> 
    <h1>Test jQuery and PHP</h1> 
        <form id="searchform" method="post"> 
    <div> 
            <label for="search_term">Search name/phone</label> 
            <input type="text" name="search_term" id="search_term" /> 
    <input type="submit" value="search" id="search_button" /> 
    </div> 
        </form> 
        <div id="search_results"></div> 
    </body> 
</html>