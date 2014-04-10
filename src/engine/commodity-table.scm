;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  commodity-table.scm
;;;  load and save commodity tables 
;;;
;;;  Bill Gribble <grib@billgribble.com> 3 Aug 2000 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(define gnc:*iso-4217-currency-file* "iso-4217-currencies.scm")

(define GNC_COMMODITY_NS_ISO "ISO4217")
(define GNC_COMMODITY_NS_NASDAQ "NASDAQ")
(define GNC_COMMODITY_NS_NYSE "NYSE")
(define GNC_COMMODITY_NS_AMEX "AMEX")
(define GNC_COMMODITY_NS_EUREX "EUREX")
(define GNC_COMMODITY_NS_MUTUAL "FUND")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  (gnc:setup-default-namespaces)
;;  make sure there are some reasonable commodity namespaces 
;;  in the engine
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (gnc:setup-default-namespaces table)
  (gnc:commodity-table-add-namespace table GNC_COMMODITY_NS_AMEX)
  (gnc:commodity-table-add-namespace table GNC_COMMODITY_NS_NYSE)
  (gnc:commodity-table-add-namespace table GNC_COMMODITY_NS_NASDAQ)
  (gnc:commodity-table-add-namespace table GNC_COMMODITY_NS_EUREX)
  (gnc:commodity-table-add-namespace table GNC_COMMODITY_NS_MUTUAL))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  (gnc:load-iso-4217-currencies)
;;  load the default table of ISO-4217 currency information. 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (gnc:load-iso-4217-currencies table)
  (let ((curr-path (%search-load-path gnc:*iso-4217-currency-file*)))
    (if curr-path
	(with-input-from-file curr-path
	  (lambda ()
	    (let loop ((form (read)))
	      (if (not (eof-object? form))
		  (begin 
		    (if (and (list? form)
			     (eq? 8 (length form)))
			(let ((fullname (list-ref form 0))
			      (unitname (list-ref form 1))
			      (partname (list-ref form 2))
			      (namespace (list-ref form 3))
			      (mnemonic (list-ref form 4))
			      (exchange-code (list-ref form 5))
			      (parts-per-unit (list-ref form 6))
			      (smallest-fraction (list-ref form 7)))
			  (if (and (string? fullname)
				   (string? unitname)
				   (string? partname)
				   (string? namespace)
				   (string? mnemonic)
				   (string? exchange-code)
				   (number? parts-per-unit)
				   (number? smallest-fraction))
			      (let ((comm 
				     (gnc:commodity-create 
				      fullname namespace 
				      mnemonic exchange-code
				      smallest-fraction)))
				(gnc:commodity-table-insert table comm)))))
		    (loop (read)))))))
	(display "Unable to load iso-4217 currency definitions\n"))))

(define (gnc:engine-commodity-table-construct table)
  (gnc:load-iso-4217-currencies table)
  (gnc:setup-default-namespaces table))