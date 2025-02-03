(function ($) {
  Backdrop.behaviors.download_count_sparklines = {
    attach: function(context, settings) {
      var options = {
        type: settings.download_count.type,
        chartRangeMin: settings.download_count.min,
        height: settings.download_count.height,
        width: settings.download_count.width,
      };

      // Apply sparklines if the function exists
      if ($.fn.sparkline) {
        $('div.download-count-sparkline-daily', context).sparkline(settings.download_count.values.daily.split(','), options);
        $('div.download-count-sparkline-weekly', context).sparkline(settings.download_count.values.weekly.split(','), options);
        $('div.download-count-sparkline-monthly', context).sparkline(settings.download_count.values.monthly.split(','), options);
        $('div.download-count-sparkline-yearly', context).sparkline(settings.download_count.values.yearly.split(','), options);
      } else {
        console.error('Sparkline library is not loaded.');
      }
    }
  };
})(jQuery);
