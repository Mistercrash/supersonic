(function($){
  Drupal.behaviors.chosen_select = {
    attach: function(context) {
          $('.chzn-container').css({width : '100%'});
          $('.chzn-drop').css({'width' : '100%'});
          
    }
  }
  Drupal.behaviors.seek = {
    attach: function(context, settings) {
    	$('#views-form-jump-overview-default tbody tr').each(function() {
            var $this = $(this);
            // Disable fields that should be disabled
        	if($this.find('input[type=checkbox]').is(':checked')) {
        		$this.addClass('point');
    	    	$this.find('.editablefield-item select').attr('disabled','disabled');
    	    	$this.find('input[type="text"]').attr('disabled','disabled');
            }
        	else {
        		$this.addClass('bust');
        	}
        	// Toggle disabled fields
            $this.find('input[type=checkbox]').change(function() {
            	if($this.find('input[type=checkbox]').is(':checked')) {
            	$this.addClass('point');
            	$this.removeClass('bust');
    	    	$this.find('.editablefield-item select').attr('disabled','disabled');
    	    	$this.find('input[type="text"]').attr('disabled','disabled');
            	}
            	else {
            	$this.removeClass('point');
            	$this.addClass('bust');
        	    $this.find('.editablefield-item select').removeAttr("disabled");
        	    $this.find('input[type="text"]').removeAttr("disabled");
            	}
            });
        });

    	$('#edit-add-point').click(function(){ 
    		var $timecode = $('#youtube-player-container').tubeplayer('data')['currentTime'];
    		if($('#edit-start-time').val() == null) {
    			// Set the start point option for the first select
    			$('#edit-start-time').append('<option value="'+ $timecode.toFixed(1) +'" selected="selected">'+ $timecode.toFixed(1) +'</option>');
    			// Update the chosen element
        		$('#edit-start-time').trigger("liszt:updated");
        		$('#edit-add-point').attr('value', 'Add point');
    		}
    		else {
    			var $sb = $('#edit-point-markers');
    			// Add the point options to the select element
    			$sb.append('<option value="'+ $timecode.toFixed(1) +'" selected="selected">'+ $timecode.toFixed(1) +'</option>');
    			// Set the total points textfield value
        		$('#edit-points-total').val($('#edit-point-markers option').size());
        		
        		// Reorder the options according to their value
        		$sb.append($sb.find('option').sort(function(a, b){
        		    return (
        		        a = parseFloat($(a).val()),
        		        b = parseFloat($(b).val()),
        		        a > b
        		    );
        		}));
        		// Update the chosen element with the new options
        		$sb.trigger("liszt:updated");


    		}
    		return false;
    	});
    	$('#edit-point-markers').change(function(){ 
        	$('#edit-points-total').val($('#edit-point-markers :selected').length);
    	});
    	$('#edit-start-time').change(function(){
    		if($('#edit-start-time').val() == null) {
        	  $('#edit-add-point').attr('value', 'Start');
    		}
    	});
    	
    	$('#edit-clear-form').click(function(){ 
    		// Clear out the options and total points
	    	$('#edit-point-markers').children().remove();
	    	$('#edit-start-time').children().remove();
	    	$('#edit-points-total').val('0');
	        // Update the chosen element with the new options
	    	$('#edit-point-markers').trigger("liszt:updated");
	    	$('#edit-start-time').trigger("liszt:updated");
	    	$('#edit-add-point').attr('value', 'Start');
    		return false;
    	});
    	
    	$("#youtube-player-container").tubeplayer({
    		width: 600, // the width of the player
    		height: 360, // the height of the player
    		allowFullScreen: "true", // true by default, allow user to go full screen
    		initialVideo: "ZqBz59tJtZw", // the video that is loaded into the player
    		preferredQuality: "hd720",// preferred quality: default, small, medium, large, hd720
    		onPlay: function(id){}, // after the play method is called
    		onPause: function(){}, // after the pause method is called
    		onStop: function(){}, // after the player is stopped
    		onSeek: function(time){}, // after the video has been seeked to a defined point
    		onMute: function(){}, // after the player is muted
    		onUnMute: function(){} // after the player is unmuted
    	});
    }
  }


})(jQuery);